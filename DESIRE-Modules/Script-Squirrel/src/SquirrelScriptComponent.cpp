#include "stdafx_Squirrel.h"
#include "SquirrelScriptComponent.h"

#include "Engine/Core/String/String.h"

#include "Engine/Utils/Enumerator.h"

SquirrelScriptComponent::SquirrelScriptComponent(Object& object, HSQUIRRELVM vm)
	: ScriptComponent(object)
	, m_vm(vm)
{
	sq_resetobject(&scriptObject);
	for(auto i : Enumerator<EBuiltinFuncType>())
	{
		sq_resetobject(&builtinFunctions[i]);
	}
}

SquirrelScriptComponent::~SquirrelScriptComponent()
{
	if(!sq_isnull(scriptObject))
	{
		sq_release(m_vm, &scriptObject);
		sq_resetobject(&scriptObject);
	}

	for(auto i : Enumerator<EBuiltinFuncType>())
	{
		if(!sq_isnull(builtinFunctions[i]))
		{
			sq_release(m_vm, &builtinFunctions[i]);
			sq_resetobject(&builtinFunctions[i]);
		}
	}
}

void SquirrelScriptComponent::CallByType(EBuiltinFuncType funcType)
{
	if(sq_isnull(builtinFunctions[(size_t)funcType]))
	{
		return;
	}

	// Save the stack size before the call
	m_savedStackTop = sq_gettop(m_vm);

	sq_pushobject(m_vm, scriptObject);
	sq_pushobject(m_vm, builtinFunctions[(size_t)funcType]);

	sq_pushobject(m_vm, scriptObject);	// the 'this' parameter
	m_numFunctionCallArgs = 1;

	ExecuteFunctionCall();
}

SQInteger SquirrelScriptComponent::CallFromScript(HSQUIRRELVM vm)
{
	// argCount is the number of arguments to call the script function with (-2 because we need to exclude the instance and the function name)
	const SQInteger argCount = sq_gettop(vm) - 2;

	SquirrelScriptComponent* scriptComp = Sqrat::Var<SquirrelScriptComponent*>(vm, -2 - argCount).value;
	ASSERT(vm == scriptComp->m_vm);

	const SQChar* functionName = nullptr;
	SQInteger size = 0;
	sq_getstringandsize(vm, -1 - argCount, &functionName, &size);

	if(scriptComp->PrepareFunctionCall(String(functionName, size)))
	{
		// Push the args
		const SQInteger argIdxOffsetFromTop = -argCount - 3;	// -3 because the object, the function and the 'this' parameter are pushed in PrepareFunctionCall()
		for(SQInteger i = 0; i < argCount; ++i)
		{
			sq_push(vm, argIdxOffsetFromTop);
			scriptComp->m_numFunctionCallArgs++;
		}

		scriptComp->ExecuteFunctionCall();
	}

	return 0;
}

bool SquirrelScriptComponent::PrepareFunctionCall(const String& functionName)
{
	// Save the stack size before the call
	m_savedStackTop = sq_gettop(m_vm);

	sq_pushobject(m_vm, scriptObject);
	sq_pushstring(m_vm, functionName.Str(), functionName.Length());
	if(SQ_FAILED(sq_get(m_vm, -2)))
	{
		sq_settop(m_vm, m_savedStackTop);
		return false;
	}

	// Verify that the function exists
	const SQObjectType type = sq_gettype(m_vm, -1);
	if(type != OT_CLOSURE && type != OT_NATIVECLOSURE)
	{
		sq_settop(m_vm, m_savedStackTop);
		return false;
	}

	sq_pushobject(m_vm, scriptObject);	// the 'this' parameter
	m_numFunctionCallArgs = 1;
	return true;
}

void SquirrelScriptComponent::ExecuteFunctionCall()
{
	const SQRESULT result = sq_call(m_vm, m_numFunctionCallArgs, SQFalse, SQTrue);
	// Restore the original stack size
	sq_settop(m_vm, m_savedStackTop);

	if(SQ_FAILED(result))
	{
		LOG_ERROR("Execution error");
	}
}

bool SquirrelScriptComponent::AddFunctionCallArg(int arg)
{
	sq_pushinteger(m_vm, arg);
	m_numFunctionCallArgs++;
	return true;
}

bool SquirrelScriptComponent::AddFunctionCallArg(float arg)
{
	sq_pushfloat(m_vm, arg);
	m_numFunctionCallArgs++;
	return true;
}

bool SquirrelScriptComponent::AddFunctionCallArg(double arg)
{
	sq_pushfloat(m_vm, (SQFloat)arg);
	m_numFunctionCallArgs++;
	return true;
}

bool SquirrelScriptComponent::AddFunctionCallArg(bool arg)
{
	sq_pushbool(m_vm, (SQBool)arg);
	m_numFunctionCallArgs++;
	return true;
}

bool SquirrelScriptComponent::AddFunctionCallArg(void* pArg)
{
	ASSERT(pArg != nullptr);

	sq_pushuserpointer(m_vm, pArg);
	m_numFunctionCallArgs++;
	return true;
}

bool SquirrelScriptComponent::AddFunctionCallArg(const String& arg)
{
	sq_pushstring(m_vm, arg.Str(), static_cast<SQInteger>(arg.Length()));
	m_numFunctionCallArgs++;
	return true;
}
