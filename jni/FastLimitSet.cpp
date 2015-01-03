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

void calculate(string xExpression, string yExpression, int width, int height, int* counterBody, double* xBody, double* yBody, int steps) {

  	ParserVariables xVars, yVars;

  	ParserNode* xExpressionRoot = NULL;
  	ParserNode* yExpressionRoot = NULL;
  	Module* mainModule = NULL;
  	ExecutionEngine* executionEngine = NULL;
  	try
  	{
		__android_log_print(ANDROID_LOG_INFO, APPNAME, "Compiling x = %s, y = %s", xExpression.c_str(), yExpression.c_str());
		InitializeNativeTarget();
		LLVMContext context;

		// Creating the main module
		mainModule = new Module("Main", context);

		Type* doubleType = Type::getDoubleTy(context);

		// Parsing the formula
		LexerTree xExpressionLexerTree(xExpression), yExpressionLexerTree(yExpression);

		ParserOperatorPriorities pop;
		Parser formulaParser(pop);

		//Type* doubleComplexType = VectorType::get(doubleType, 2);


		// Creating the functions
		// double xFunction(x, y, x0, y0)
		Function *xFunction = cast<Function>(mainModule->getOrInsertFunction("calcX", doubleType, doubleType, doubleType, doubleType, doubleType, (Type *)0));
		{
			Function::arg_iterator xFuncArgs = xFunction->arg_begin();
			xFuncArgs->setName("x");
			xVars.defineExternal("x", tDouble, xFuncArgs);
			xFuncArgs++;
			xFuncArgs->setName("y");
			xVars.defineExternal("y", tDouble, xFuncArgs);
			xFuncArgs++;
			xFuncArgs->setName("x0");
			xVars.defineExternal("x0", tDouble, xFuncArgs);
			xFuncArgs++;
			xFuncArgs->setName("y0");
			xVars.defineExternal("y0", tDouble, xFuncArgs);
			xFuncArgs++;
		}
		BasicBlock *xFunctionEntryBlock = BasicBlock::Create(context, "calcX_entry_block", xFunction);
		IRBuilder<> builder(xFunctionEntryBlock);
		xExpressionRoot = formulaParser.parseFlow(xExpressionLexerTree.getRoot().getInnerItems(), xVars);
		Value* xFormulaVal = xExpressionRoot->generateGetValueLLVMCode(builder);

		// double yFunction(x, y, x0, y0)
		Function *yFunction = cast<Function>(mainModule->getOrInsertFunction("calcY", doubleType, doubleType, doubleType, doubleType, doubleType, (Type *)0));
		{
			Function::arg_iterator yFuncArgs = yFunction->arg_begin();
			yFuncArgs->setName("x");
			yVars.defineExternal("x", tDouble, yFuncArgs);
			yFuncArgs++;
			yFuncArgs->setName("y");
			yVars.defineExternal("y", tDouble, yFuncArgs);
			yFuncArgs++;
			yFuncArgs->setName("x0");
			yVars.defineExternal("x0", tDouble, yFuncArgs);
			yFuncArgs++;
			yFuncArgs->setName("y0");
			yVars.defineExternal("y0", tDouble, yFuncArgs);
			yFuncArgs++;
		}
		BasicBlock *yFunctionEntryBlock = BasicBlock::Create(context, "calcY_entry_block", yFunction);
		builder.SetInsertPoint(yFunctionEntryBlock);
		yExpressionRoot = formulaParser.parseFlow(yExpressionLexerTree.getRoot().getInnerItems(), yVars);
		Value* yFormulaVal = yExpressionRoot->generateGetValueLLVMCode(builder);


		// ** JITing and executing **

		// Now we create the JIT.
		executionEngine = EngineBuilder(mainModule).create();

		// Call the `foo' function with no arguments:
		std::vector<GenericValue> args;
		args.push_back(GenericValue());
		args.push_back(GenericValue());
		args.push_back(GenericValue());
		args.push_back(GenericValue());

		__android_log_print(ANDROID_LOG_INFO, APPNAME, "Compiled successfully");
		for (int i = 0; i < width; i++) {
			for (int j = 0; j < height; j++) {

				double x = xBody[j * width + i];
				double y = yBody[j * width + i];
				int counterVal = counterBody[j * width + i];

				double x0 = (4.0 / width) * i - 2;
				double y0 = (4.0 / height) * j - 2;

				//GenericValue gv;

				for (int step = 0; step < steps; step++) {
					if (x*x + y*y < 1000000.0) {
						args[0].DoubleVal = x;
						args[1].DoubleVal = y;
						args[2].DoubleVal = x0;
						args[3].DoubleVal = y0;

						GenericValue xValue = executionEngine->runFunction(xFunction, args);
						GenericValue yValue = executionEngine->runFunction(yFunction, args);

						x = xValue.DoubleVal;
						y = yValue.DoubleVal;
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



		// Import result of execution:
		executionEngine->freeMachineCodeForFunction(xFunction);
		executionEngine->freeMachineCodeForFunction(yFunction);
  	}
  	catch (const LexerException& lexerException)
  	{
  		printf("Lexer is asking for excuse. %s\n", lexerException.getMessage().c_str());
  		fflush(stdout);
  		//return -1;
  	}
  	catch (const ParserException& parserException)
  	{
  		printf("Parser is asking for excuse. %s\n", parserException.getMessage().c_str());
  		fflush(stdout);
  		//return -1;
  	}

  	if (xExpressionRoot != NULL) delete xExpressionRoot;
  	if (yExpressionRoot != NULL) delete yExpressionRoot;

	llvm_shutdown();
	//return 0;
}

extern "C" {
	/*JNIEXPORT void JNICALL Java_bfbc_toys_fastlimitset_NativeLimitSetCalculator_doSteps(JNIEnv* env, jclass clz, jint width, jint height, jintArray counter, jdoubleArray x, jdoubleArray y, jint steps) {
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

	}*/

	JNIEXPORT void JNICALL Java_bfbc_toys_fastlimitset_NativeLimitSetCalculator_doStepsExpression(JNIEnv* env, jclass clz, jstring xExpression, jstring yExpression, jint width, jint height, jintArray counter, jdoubleArray x, jdoubleArray y, jint steps) {
	    const char *xExpressionStr, *yExpressionStr;
	    xExpressionStr = env->GetStringUTFChars(xExpression, NULL);
	    yExpressionStr = env->GetStringUTFChars(yExpression, NULL);

	    jint *counterBody = env->GetIntArrayElements(counter, 0);
		jdouble *xBody = env->GetDoubleArrayElements(x, 0);
		jdouble *yBody = env->GetDoubleArrayElements(y, 0);

		calculate((string)xExpressionStr, (string)yExpressionStr, width, height, counterBody, xBody, yBody, steps);

		env->ReleaseIntArrayElements(counter, counterBody, 0);
		env->ReleaseDoubleArrayElements(x, xBody, 0);
		env->ReleaseDoubleArrayElements(y, yBody, 0);

	}

}
