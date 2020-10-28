#include "Engine/stdafx.h"
#include "Engine/Script/NativeScriptComponent.h"

#include "Engine/Core/String/String.h"

NativeScriptComponent::NativeScriptComponent(Object& object, ScriptSystem::ScriptFactory::Func_t scriptFactoryFunc)
	: ScriptComponent(object)
{
	m_spScript = scriptFactoryFunc(*this);
	ASSERT(m_spScript != nullptr);
}

NativeScriptComponent::~NativeScriptComponent()
{
}

void NativeScriptComponent::CallByType(EBuiltinFuncType funcType)
{
	switch(funcType)
	{
		case EBuiltinFuncType::Update:	m_spScript->Update(); break;
		case EBuiltinFuncType::Init:	m_spScript->Init(); break;
		case EBuiltinFuncType::Kill:	m_spScript->Kill(); break;
		case EBuiltinFuncType::Num:		break;
	};
}

bool NativeScriptComponent::PrepareFunctionCall(const String& functionName)
{
	m_functionToCall = m_spScript->GetFunctionToCall(functionName);
	if(m_functionToCall == nullptr)
	{
		return false;
	}

	m_functionArgs.fill({ IScript::Arg::EType::Undefined, 0 });
	m_numFunctionCallArgs = 0;
	return true;
}

void NativeScriptComponent::ExecuteFunctionCall()
{
	ASSERT(m_functionToCall != nullptr);
	m_functionToCall(m_functionArgs);
	m_functionToCall = nullptr;
}

bool NativeScriptComponent::AddFunctionCallArg(int arg)
{
	ASSERT(m_numFunctionCallArgs < m_functionArgs.max_size());

	m_functionArgs[m_numFunctionCallArgs].type = IScript::Arg::EType::Int;
	m_functionArgs[m_numFunctionCallArgs].inVal = arg;
	m_numFunctionCallArgs++;
	return true;
}

bool NativeScriptComponent::AddFunctionCallArg(float arg)
{
	ASSERT(m_numFunctionCallArgs < m_functionArgs.max_size());

	m_functionArgs[m_numFunctionCallArgs].type = IScript::Arg::EType::Float;
	m_functionArgs[m_numFunctionCallArgs].floatVal = arg;
	m_numFunctionCallArgs++;
	return true;
}

bool NativeScriptComponent::AddFunctionCallArg(double arg)
{
	ASSERT(m_numFunctionCallArgs < m_functionArgs.max_size());

	m_functionArgs[m_numFunctionCallArgs].type = IScript::Arg::EType::Double;
	m_functionArgs[m_numFunctionCallArgs].doubleVal = arg;
	m_numFunctionCallArgs++;
	return true;
}

bool NativeScriptComponent::AddFunctionCallArg(bool arg)
{
	ASSERT(m_numFunctionCallArgs < m_functionArgs.max_size());

	m_functionArgs[m_numFunctionCallArgs].type = IScript::Arg::EType::Bool;
	m_functionArgs[m_numFunctionCallArgs].boolVal = arg;
	m_numFunctionCallArgs++;
	return true;
}

bool NativeScriptComponent::AddFunctionCallArg(void* pArg)
{
	ASSERT(m_numFunctionCallArgs < m_functionArgs.max_size());

	m_functionArgs[m_numFunctionCallArgs].type = IScript::Arg::EType::VoidPtr;
	m_functionArgs[m_numFunctionCallArgs].voidPtrVal = pArg;
	m_numFunctionCallArgs++;
	return true;
}

bool NativeScriptComponent::AddFunctionCallArg(const String& arg)
{
	ASSERT(m_numFunctionCallArgs < m_functionArgs.max_size());

	m_functionArgs[m_numFunctionCallArgs].type = IScript::Arg::EType::String;
	m_functionArgs[m_numFunctionCallArgs].stringVal = arg.Str();
	m_numFunctionCallArgs++;
	return true;
}
