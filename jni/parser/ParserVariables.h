/*
 * ParserVariables.h
 *
 *  Created on: Apr 8, 2013
 *      Author: imizus
 */

#ifndef PARSERVARIABLES_H_
#define PARSERVARIABLES_H_

#include <map>
#include <string>

#include <llvm/IR/Value.h>
#include <llvm/IR/IRBuilder.h>

#include "ParserException.h"
#include "DataType.h"

using namespace std;
using namespace llvm;

class ParserVariables
{
public:
	struct Variable
	{
		bool needsLoading;
		DataType type;
		Value* value;
		Variable(DataType type, Value* value, bool needsLoading) : type(type), value(value), needsLoading(needsLoading) {}
	};
private:
	map<string, Variable> values;

protected:
	const Variable& getLLVMVariableIdentifier(const string& name) const
	{
		map<string, Variable>::const_iterator iter = values.find(name);
		if (iter == values.end())
		{
			throw UndefinedVariableParserException(name);
		}
		else
		{
			return (*iter).second;
		}
	}
public:
	void define(const string& name, DataType type, bool needsLoading = true) {
		if (values.find(name) == values.end())
		{
			values.insert(pair<string, Variable>(name, Variable(type, NULL, needsLoading)));
		}
		else
		{
			throw VariableRedifinitionParserException(name);
		}
	}

	void defineExternal(const string& name, DataType type, Value* value, bool needsLoading = false) {
		if (values.find(name) == values.end())
		{
			values.insert(pair<string, Variable>(name, Variable(type, value, needsLoading)));
		}
		else
		{
			throw VariableRedifinitionParserException(name);
		}
	}

	void generateVariableCreationLLVMCode(const string& name, DataType type, IRBuilder<>& builder)
	{
		map<string, Variable>::iterator iter = values.find(name);
		if (iter != values.end() && (*iter).second.type == type)
		{
		  	AllocaInst* inst = builder.CreateAlloca(variableTypeToLLVMType(type, builder), 0, name);

		  	// Saving the value for the new variable
		  	(*iter).second = Variable(type, inst, true);
		}
		else if (iter != values.end())
		{
			throw InvalidVariableTypeParserException(name);
		}
		else
		{
			throw UndefinedVariableParserException(name);
		}
	}

	StoreInst* generateLLVMVariableBooleanSetToConstantCode(const string& name, bool value, IRBuilder<>& builder) const
	{
		const Variable& var = getLLVMVariableIdentifier(name);
		if (var.type == tBoolean)
		{
			Type* booleanType = builder.getInt1Ty();
			Value* xValue = value ? ConstantInt::getTrue(booleanType) : ConstantInt::getFalse(booleanType);
			return builder.CreateStore(xValue, var.value);
		}
		else
		{
			throw InvalidVariableTypeParserException(name);
		}
	}

	StoreInst* generateLLVMVariableDoubleSetToConstantCode(const string& name, double value, IRBuilder<>& builder) const
	{
		const Variable& var = getLLVMVariableIdentifier(name);
		if (var.type == tDouble)
		{
			Type* doubleType = builder.getDoubleTy();
			Value* xValue = ConstantFP::get(doubleType, value);
			return builder.CreateStore(xValue, var.value);
		}
		else
		{
			throw InvalidVariableTypeParserException(name);
		}
	}

	StoreInst* generateLLVMVariableSetValueCode(const string& name, DataType type, Value* value, IRBuilder<>& builder) const
	{
		if (getLLVMVariableIdentifier(name).type == type)
		{
			return builder.CreateStore(value, getLLVMVariableIdentifier(name).value);
		}
		else
		{
			throw InvalidVariableTypeParserException(name);
		}
	}

	Value* generateLLVMVariableGetValueCode(const string& name, DataType type, IRBuilder<>& builder) const
	{
		if (getLLVMVariableIdentifier(name).type == type)
		{
			Variable var = getLLVMVariableIdentifier(name);
			if (var.needsLoading) {
				return builder.CreateLoad(getLLVMVariableIdentifier(name).value, name);
			} else {
				return var.value;
			}
		}
		else
		{
			throw InvalidVariableTypeParserException(name);
		}
	}

	bool contains(const string& name, DataType type) const
	{
		map<string, Variable>::const_iterator iter = values.find(name);
		return iter != values.end() && iter->second.type == type;
	}
};


#endif /* PARSERVARIABLES_H_ */
