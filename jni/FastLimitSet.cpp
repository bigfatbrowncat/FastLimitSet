#include <stdio.h>
#include <string>
#include <jni.h>

#include <android/log.h>
#define APPNAME "FastLimitSet native"

#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

//#include "coffeecatch/coffeecatch.h"

#include <llvm/IR/Constants.h>
#include <llvm/ADT/Triple.h>
#include <llvm/Support/Host.h>
#include "llvm/Support/TargetRegistry.h"
#include <llvm/PassRegistry.h>
#include <llvm/InitializePasses.h>
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
//#include "llvm/ExecutionEngine/JIT.h"
#include "llvm/ExecutionEngine/Interpreter.h"
#include "llvm/ExecutionEngine/MCJIT.h"
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

// x*x + y*y < sqrDistance
Value* notRunawayCondition(IRBuilder<> b, Value* x, Value* y, Value* sqrDistance) {
	Value* xMulX = b.CreateFMul(x, x, "x^2");
	Value* yMulY = b.CreateFMul(y, y, "y^2");
	Value* sum = b.CreateFAdd(xMulX, yMulY, "x^2 + y^2");
	Value* grt = b.CreateFCmpOLT(sum, sqrDistance);
	return grt;
}

bool calculate(string expression, int width, int height, int* counterBody, double* xBody, double* yBody, int steps) {
	ParserVariables vars;

  	ParserNode* expressionRoot = NULL;
  	Module* mainModule = NULL;
  	ExecutionEngine* executionEngine = NULL;
  	try
  	{
		__android_log_print(ANDROID_LOG_INFO, APPNAME, "Compiling %s", expression.c_str());
		LLVMContext context;

		mainModule = new Module("Main", context);

		Type* doubleType = Type::getDoubleTy(context);
		Type* intType = Type::getInt32Ty(context);

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
			BasicBlock *formulaEntryBlock = BasicBlock::Create(context, "formula_entry", formula);

			IRBuilder<> builder(formulaEntryBlock);

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

		InitializeNativeTarget();

		string defTargetTriple = llvm::sys::getDefaultTargetTriple();
		string targetTriple = llvm::sys::getProcessTriple();
		string hostName = llvm::sys::getHostCPUName().str();

		//Triple* cur = new Triple();
		__android_log_print(ANDROID_LOG_INFO, APPNAME, "Default target triple: %s, process triple: %s, host CPU: %s", defTargetTriple.c_str(), targetTriple.c_str(), hostName.c_str());

		__android_log_print(ANDROID_LOG_INFO, APPNAME, "Listing the registered targets:");
		for (TargetRegistry::iterator iter = TargetRegistry::begin(); iter != TargetRegistry::end(); iter++)
		{
			__android_log_print(ANDROID_LOG_INFO, APPNAME, "A registered target: %s, has jit? %d, has target machine? %d", iter->getName(), iter->hasJIT(), iter->hasTargetMachine());
		}

		string err;
		const Target* found = TargetRegistry::lookupTarget("arm-unknown-linux-androideabi", err);
		if (found != NULL)
		{
			__android_log_print(ANDROID_LOG_INFO, APPNAME, "The found arm target: %s", found->getName());
		}
		else
		{
			__android_log_print(ANDROID_LOG_INFO, APPNAME, "Error while searching arm target: %s", err.c_str());
		}


		EngineBuilder engineBuilder(mainModule);

		std::string errStr;
		executionEngine =
			engineBuilder
			.setErrorStr(&errStr)
			/*.setUseMCJIT(true)
			.setEngineKind(EngineKind::JIT)*/
			.setEngineKind(EngineKind::Interpreter)
			.create();



		if (executionEngine != NULL)
		{
			__android_log_print(ANDROID_LOG_INFO, APPNAME, "Compiled successfully");
//			__android_log_print(ANDROID_LOG_INFO, APPNAME, "Running with engine for triple: %s", executionEngine->getTargetMachine()->getTargetTriple().str().c_str());
			for (int i = 0; i < width; i++) {
				//__android_log_print(ANDROID_LOG_INFO, APPNAME, "1");
				for (int j = 0; j < height; j++) {
					//__android_log_print(ANDROID_LOG_INFO, APPNAME, "2");

					double x = xBody[j * width + i];
					double y = yBody[j * width + i];
					//__android_log_print(ANDROID_LOG_INFO, APPNAME, "3");

					int counterVal = counterBody[j * width + i];
					//__android_log_print(ANDROID_LOG_INFO, APPNAME, "4");

					double x0 = (4.0 / width) * i - 2;
					double y0 = (4.0 / height) * j - 2;
					//__android_log_print(ANDROID_LOG_INFO, APPNAME, "5");

					std::vector<GenericValue> args;
					args.push_back(GenericValue());
					args.push_back(GenericValue());
					args[0].DoubleVal = x0;
					args[1].DoubleVal = y0;
					//__android_log_print(ANDROID_LOG_INFO, APPNAME, "6");

					double* pxGV = (double*)executionEngine->getPointerToGlobal(xGV);
					double* pyGV = (double*)executionEngine->getPointerToGlobal(yGV);

					//__android_log_print(ANDROID_LOG_INFO, APPNAME, "7");
					for (int step = 0; step < steps; step ++) {
						if (x*x + y*y < 1000000.0) {

							*pxGV = x;
							*pyGV = y;
							//__android_log_print(ANDROID_LOG_INFO, APPNAME, "8");

							GenericValue res = executionEngine->runFunction(formula, args);
							//__android_log_print(ANDROID_LOG_INFO, APPNAME, "9");

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
			return true;
		}
		else
		{
	  		__android_log_print(ANDROID_LOG_ERROR, APPNAME, "LLVM can't initialize the specified execution engine. Cause: %s", errStr.c_str());
		}
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

	__android_log_print(ANDROID_LOG_INFO, APPNAME, "Ok or not ok?");
	llvm_shutdown();
	return true;
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

	JNIEXPORT jboolean JNICALL Java_bfbc_toys_fastlimitset_NativeLimitSetCalculator_doStepsExpression(JNIEnv* env, jclass clz, jstring expression, jint width, jint height, jintArray counter, jdoubleArray x, jdoubleArray y, jint steps) {
	    const char *expressionStr;
	    expressionStr = env->GetStringUTFChars(expression, NULL);

	    jint *counterBody = env->GetIntArrayElements(counter, 0);
		jdouble *xBody = env->GetDoubleArrayElements(x, 0);
		jdouble *yBody = env->GetDoubleArrayElements(y, 0);

		bool res;
		//COFFEE_TRY()
		{
			res = calculate((string)expressionStr, width, height, counterBody, xBody, yBody, steps);
		}
		//COFFEE_CATCH()
		{
			/*const char* const message = coffeecatch_get_message();
			jclass cls = env->FindClass("java/lang/RuntimeException");
			env->ThrowNew(cls, strdup(message));
			res = false;*/
		} //COFFEE_END();


		env->ReleaseIntArrayElements(counter, counterBody, 0);
		env->ReleaseDoubleArrayElements(x, xBody, 0);
		env->ReleaseDoubleArrayElements(y, yBody, 0);

		return res;
	}

}
