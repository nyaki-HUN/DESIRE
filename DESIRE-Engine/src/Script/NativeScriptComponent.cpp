#include "stdafx.h"
#include "Script/NativeScriptComponent.h"
#include "Core/String.h"

NativeScriptComponent::NativeScriptComponent(std::unique_ptr<IScript> i_script)
	: script(std::move(i_script))
	, functionToCall(nullptr)
	, numFunctionCallArgs(0)
{
	ASSERT(script != nullptr);
	script->self = this;
}

NativeScriptComponent::~NativeScriptComponent()
{

}

void NativeScriptComponent::CallByType(EBuiltinFuncType funcType)
{
	switch(funcType)
	{
		case EBuiltinFuncType::UPDATE:	script->Update(); break;
		case EBuiltinFuncType::INIT:	script->Init(); break;
		case EBuiltinFuncType::KILL:	script->Kill(); break;
		case EBuiltinFuncType::NUM:		break;
	};
}

bool NativeScriptComponent::PrepareFunctionCall(const char *functionName)
{
	functionToCall = script->GetFunctionToCall(functionName);
	if(functionToCall == nullptr)
	{
		return false;
	}

	functionArgs.fill({ IScript::SArg::EType::UNDEFINED, 0 });
	numFunctionCallArgs = 0;
	return true;
}

void NativeScriptComponent::ExecuteFunctionCall()
{
	ASSERT(functionToCall != nullptr);
	functionToCall(functionArgs);
	functionToCall = nullptr;
}

bool NativeScriptComponent::AddFunctionCallArg(int arg)
{
	ASSERT(numFunctionCallArgs < functionArgs.max_size());

	functionArgs[numFunctionCallArgs].type = IScript::SArg::EType::INT;
	functionArgs[numFunctionCallArgs].inVal = arg;
	numFunctionCallArgs++;
	return true;
}

bool NativeScriptComponent::AddFunctionCallArg(float arg)
{
	ASSERT(numFunctionCallArgs < functionArgs.max_size());

	functionArgs[numFunctionCallArgs].type = IScript::SArg::EType::FLOAT;
	functionArgs[numFunctionCallArgs].floatVal = arg;
	numFunctionCallArgs++;
	return true;
}

bool NativeScriptComponent::AddFunctionCallArg(double arg)
{
	ASSERT(numFunctionCallArgs < functionArgs.max_size());

	functionArgs[numFunctionCallArgs].type = IScript::SArg::EType::DOUBLE;
	functionArgs[numFunctionCallArgs].doubleVal = arg;
	numFunctionCallArgs++;
	return true;
}

bool NativeScriptComponent::AddFunctionCallArg(bool arg)
{
	ASSERT(numFunctionCallArgs < functionArgs.max_size());

	functionArgs[numFunctionCallArgs].type = IScript::SArg::EType::BOOL;
	functionArgs[numFunctionCallArgs].boolVal = arg;
	numFunctionCallArgs++;
	return true;
}

bool NativeScriptComponent::AddFunctionCallArg(void *arg)
{
	ASSERT(numFunctionCallArgs < functionArgs.max_size());

	functionArgs[numFunctionCallArgs].type = IScript::SArg::EType::VOID_PTR;
	functionArgs[numFunctionCallArgs].voidPtrVal = arg;
	numFunctionCallArgs++;
	return true;
}

bool NativeScriptComponent::AddFunctionCallArg(const String& arg)
{
	ASSERT(numFunctionCallArgs < functionArgs.max_size());

	functionArgs[numFunctionCallArgs].type = IScript::SArg::EType::STRING;
	functionArgs[numFunctionCallArgs].stringVal = arg.c_str();
	numFunctionCallArgs++;
	return true;
}
