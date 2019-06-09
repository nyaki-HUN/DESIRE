#include "Engine/stdafx.h"
#include "Engine/Script/NativeScriptComponent.h"
#include "Engine/Core/String/String.h"

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
		case EBuiltinFuncType::Update:	script->Update(); break;
		case EBuiltinFuncType::Init:	script->Init(); break;
		case EBuiltinFuncType::Kill:	script->Kill(); break;
		case EBuiltinFuncType::Num:		break;
	};
}

bool NativeScriptComponent::PrepareFunctionCall(const char* functionName)
{
	functionToCall = script->GetFunctionToCall(functionName);
	if(functionToCall == nullptr)
	{
		return false;
	}

	functionArgs.fill({ IScript::Arg::EType::Undefined, 0 });
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

	functionArgs[numFunctionCallArgs].type = IScript::Arg::EType::Int;
	functionArgs[numFunctionCallArgs].inVal = arg;
	numFunctionCallArgs++;
	return true;
}

bool NativeScriptComponent::AddFunctionCallArg(float arg)
{
	ASSERT(numFunctionCallArgs < functionArgs.max_size());

	functionArgs[numFunctionCallArgs].type = IScript::Arg::EType::Float;
	functionArgs[numFunctionCallArgs].floatVal = arg;
	numFunctionCallArgs++;
	return true;
}

bool NativeScriptComponent::AddFunctionCallArg(double arg)
{
	ASSERT(numFunctionCallArgs < functionArgs.max_size());

	functionArgs[numFunctionCallArgs].type = IScript::Arg::EType::Double;
	functionArgs[numFunctionCallArgs].doubleVal = arg;
	numFunctionCallArgs++;
	return true;
}

bool NativeScriptComponent::AddFunctionCallArg(bool arg)
{
	ASSERT(numFunctionCallArgs < functionArgs.max_size());

	functionArgs[numFunctionCallArgs].type = IScript::Arg::EType::Bool;
	functionArgs[numFunctionCallArgs].boolVal = arg;
	numFunctionCallArgs++;
	return true;
}

bool NativeScriptComponent::AddFunctionCallArg(void* arg)
{
	ASSERT(numFunctionCallArgs < functionArgs.max_size());

	functionArgs[numFunctionCallArgs].type = IScript::Arg::EType::VoidPtr;
	functionArgs[numFunctionCallArgs].voidPtrVal = arg;
	numFunctionCallArgs++;
	return true;
}

bool NativeScriptComponent::AddFunctionCallArg(const String& arg)
{
	ASSERT(numFunctionCallArgs < functionArgs.max_size());

	functionArgs[numFunctionCallArgs].type = IScript::Arg::EType::String;
	functionArgs[numFunctionCallArgs].stringVal = arg.Str();
	numFunctionCallArgs++;
	return true;
}
