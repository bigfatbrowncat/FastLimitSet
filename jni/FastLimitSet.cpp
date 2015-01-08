#include <stdio.h>
#include <string>
#include <jni.h>

#include <android/log.h>
#define APPNAME "FastLimitSet native"

#include <llvm/IR/Constants.h>
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/ExecutionEngine/JIT.h"
#include "llvm/ExecutionEngine/Interpreter.h"
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/ManagedStatic.h"
#include "llvm/Support/raw_ostream.h"

#include "lexer/LexerException.h"

#include "parser/Parser.h"
#include "parser/ParserException.h"

#include "FastLimitSet.h"

using namespace std;
using namespace llvm;

// Needed by LLVM
extern "C" double atof(const char *nptr)
{
    return (strtod(nptr, NULL));
}

void calculate(string expression, int width, int height, int* counterBody, double* xBody, double* yBody, int steps) {

  	ParserVariables vars;

  	ParserNode* expressionRoot = NULL;
  	Module* mainModule = NULL;
  	ExecutionEngine* executionEngine = NULL;
  	try
  	{
		__android_log_print(ANDROID_LOG_INFO, APPNAME, "Compiling %s", expression.c_str());
		InitializeNativeTarget();
		LLVMContext context;

		mainModule = new Module("Main", context);

		Type* doubleType = Type::getDoubleTy(context);

		LexerTree xExpressionLexerTree(expression);

		ParserOperatorPriorities pop;
		Parser formulaParser(pop);

		GlobalVariable* xGV = new GlobalVariable(*mainModule, doubleType,
						false, GlobalValue::ExternalLinkage,
						ConstantFP::get(doubleType, 0.0), "xG");
		GlobalVariable* yGV = new GlobalVariable(*mainModule, doubleType,
						false, GlobalValue::ExternalLinkage,
						ConstantFP::get(doubleType, 0.0), "yG");

		// double formula(x, y, x0, y0)
		Function *formula = cast<Function>(mainModule->getOrInsertFunction("formula", Type::getVoidTy(context), doubleType, doubleType, (Type *)0));
		{
			// Storing function argumnts
			Function::arg_iterator xFuncArgs = formula->arg_begin();
			vars.defineExternal("x0", tDouble, xFuncArgs++);
			vars.defineExternal("y0", tDouble, xFuncArgs);

			// Defining local variables
			vars.defineExternal("x", tDouble, xGV, true);
			vars.defineExternal("y", tDouble, yGV, true);
			vars.define("xn", tDouble, true);
			vars.define("yn", tDouble, true);

			// Parsing the user formula
			expressionRoot = formulaParser.parseFlow(xExpressionLexerTree.getRoot().getInnerItems(), vars);

			// Building code
			BasicBlock *xFunctionEntryBlock = BasicBlock::Create(context, "formula_entry", formula);
			IRBuilder<> builder(xFunctionEntryBlock);

			// Generating variables initialization
			vars.generateVariableCreationLLVMCode("xn", tDouble, builder);
			vars.generateVariableCreationLLVMCode("yn", tDouble, builder);

			// Generating the formula code
			Value* xFormulaVal = expressionRoot->generateGetValueLLVMCode(builder);

			builder.CreateStore(vars.generateLLVMVariableGetValueCode("xn", tDouble, builder), xGV, true);
			builder.CreateStore(vars.generateLLVMVariableGetValueCode("yn", tDouble, builder), yGV, true);

			builder.CreateRetVoid();
		}

		{
			string listStr;
			raw_string_ostream listStream(listStr);
			mainModule->print(listStream, NULL);
			__android_log_print(ANDROID_LOG_INFO, APPNAME, "[ FORMULA ]\n%s", listStr.c_str());
		}

		executionEngine = EngineBuilder(mainModule).create();

		__android_log_print(ANDROID_LOG_INFO, APPNAME, "Compiled successfully");
		for (int i = 0; i < width; i++) {
			for (int j = 0; j < height; j++) {

				double x = xBody[j * width + i];
				double y = yBody[j * width + i];

				int counterVal = counterBody[j * width + i];

				double x0 = (4.0 / width) * i - 2;
				double y0 = (4.0 / height) * j - 2;

				std::vector<GenericValue> args;
				args.push_back(GenericValue());
				args.push_back(GenericValue());
				args[0].DoubleVal = x0;
				args[1].DoubleVal = y0;

				double* pxGV = (double*)executionEngine->getPointerToGlobal(xGV);
				double* pyGV = (double*)executionEngine->getPointerToGlobal(yGV);

				for (int step = 0; step < steps; step ++) {
					if (x*x + y*y < 1000000.0) {

						*pxGV = x;
						*pyGV = y;

						GenericValue res = executionEngine->runFunction(formula, args);

						x = *pxGV;
						y = *pyGV;

						counterVal ++;
					} else {
						break;
					}
				}

				//__android_log_print(ANDROID_LOG_INFO, APPNAME, "x: %lf, y: %lf", x, y);


				xBody[j * width + i] = x;
				yBody[j * width + i] = y;
				counterBody[j * width + i] = counterVal;
			}
		}



		// Import result of execution:
		executionEngine->freeMachineCodeForFunction(formula);
  	}
  	catch (const LexerException& lexerException)
  	{
  		__android_log_print(ANDROID_LOG_ERROR, APPNAME, "Lexer is asking for excuse. %s\n", lexerException.getMessage().c_str());
  	}
  	catch (const ParserException& parserException)
  	{
  		__android_log_print(ANDROID_LOG_ERROR, APPNAME, "Parser is asking for excuse. %s\n", parserException.getMessage().c_str());
  	}
  	catch (...)
  	{
  		__android_log_print(ANDROID_LOG_ERROR, APPNAME, "Unknown exception");
  	}

  	if (expressionRoot != NULL) delete expressionRoot;

	llvm_shutdown();
	//return 0;
}

extern "C" {
	JNIEXPORT void JNICALL Java_bfbc_toys_fastlimitset_NativeLimitSetCalculator_doStepsMandelbrot(JNIEnv* env, jclass clz, jint width, jint height, jintArray counter, jdoubleArray x, jdoubleArray y, jint steps) {
		jint *counterBody = env->GetIntArrayElements(counter, 0);
		jdouble *xBody = env->GetDoubleArrayElements(x, 0);
		jdouble *yBody = env->GetDoubleArrayElements(y, 0);

		for (int i = 0; i < width; i++) {
			for (int j = 0; j < height; j++) {

				double x = xBody[j * width + i];
				double y = yBody[j * width + i];
				int counterVal = counterBody[j * width + i];

				double x0 = (4.0 / width) * i - 2;
				double y0 = (4.0 / height) * j - 2;

				for (int step = 0; step < steps; step++) {
					if (x*x + y*y < 1000000.0) {
						double x1 = x*x - y*y + x0;
						double y1 = 2*x*y + y0;
						x = x1; y = y1;
						counterVal ++;
					} else {
						break;
					}
				}

				xBody[j * width + i] = x;
				yBody[j * width + i] = y;
				counterBody[j * width + i] = counterVal;
			}
		}

		env->ReleaseIntArrayElements(counter, counterBody, 0);
		env->ReleaseDoubleArrayElements(x, xBody, 0);
		env->ReleaseDoubleArrayElements(y, yBody, 0);

	}

	JNIEXPORT void JNICALL Java_bfbc_toys_fastlimitset_NativeLimitSetCalculator_doStepsExpression(JNIEnv* env, jclass clz, jstring expression, jint width, jint height, jintArray counter, jdoubleArray x, jdoubleArray y, jint steps) {
	    const char *expressionStr;
	    expressionStr = env->GetStringUTFChars(expression, NULL);

	    jint *counterBody = env->GetIntArrayElements(counter, 0);
		jdouble *xBody = env->GetDoubleArrayElements(x, 0);
		jdouble *yBody = env->GetDoubleArrayElements(y, 0);

		calculate((string)expressionStr, width, height, counterBody, xBody, yBody, steps);

		env->ReleaseIntArrayElements(counter, counterBody, 0);
		env->ReleaseDoubleArrayElements(x, xBody, 0);
		env->ReleaseDoubleArrayElements(y, yBody, 0);

	}

}
