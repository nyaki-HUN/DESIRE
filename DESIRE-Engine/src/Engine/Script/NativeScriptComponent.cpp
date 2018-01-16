#include "Engine/stdafx.h"
#include "Engine/Script/NativeScriptComponent.h"
#include "Engine/Core/String.h"

NativeScriptComponent::NativeScriptComponent(Object& object, std::unique_ptr<IScript>&& i_script)
	: ScriptComponent(object)
	, script(std::move(i_script))
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

	functionArgs.fill({ IScript::Arg::EType::UNDEFINED, 0 });
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

	functionArgs[numFunctionCallArgs].type = IScript::Arg::EType::INT;
	functionArgs[numFunctionCallArgs].inVal = arg;
	numFunctionCallArgs++;
	return true;
}

bool NativeScriptComponent::AddFunctionCallArg(float arg)
{
	ASSERT(numFunctionCallArgs < functionArgs.max_size());

	functionArgs[numFunctionCallArgs].type = IScript::Arg::EType::FLOAT;
	functionArgs[numFunctionCallArgs].floatVal = arg;
	numFunctionCallArgs++;
	return true;
}

bool NativeScriptComponent::AddFunctionCallArg(double arg)
{
	ASSERT(numFunctionCallArgs < functionArgs.max_size());

	functionArgs[numFunctionCallArgs].type = IScript::Arg::EType::DOUBLE;
	functionArgs[numFunctionCallArgs].doubleVal = arg;
	numFunctionCallArgs++;
	return true;
}

bool NativeScriptComponent::AddFunctionCallArg(bool arg)
{
	ASSERT(numFunctionCallArgs < functionArgs.max_size());

	functionArgs[numFunctionCallArgs].type = IScript::Arg::EType::BOOL;
	functionArgs[numFunctionCallArgs].boolVal = arg;
	numFunctionCallArgs++;
	return true;
}

bool NativeScriptComponent::AddFunctionCallArg(void *arg)
{
	ASSERT(numFunctionCallArgs < functionArgs.max_size());

	functionArgs[numFunctionCallArgs].type = IScript::Arg::EType::VOID_PTR;
	functionArgs[numFunctionCallArgs].voidPtrVal = arg;
	numFunctionCallArgs++;
	return true;
}

bool NativeScriptComponent::AddFunctionCallArg(const String& arg)
{
	ASSERT(numFunctionCallArgs < functionArgs.max_size());

	functionArgs[numFunctionCallArgs].type = IScript::Arg::EType::STRING;
	functionArgs[numFunctionCallArgs].stringVal = arg.c_str();
	numFunctionCallArgs++;
	return true;
}
