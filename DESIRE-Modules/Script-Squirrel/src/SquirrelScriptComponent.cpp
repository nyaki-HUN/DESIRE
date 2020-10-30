#include "stdafx_Squirrel.h"
#include "SquirrelScriptComponent.h"

#include "Engine/Core/String/String.h"

#include "Engine/Utils/Enumerator.h"

SquirrelScriptComponent::SquirrelScriptComponent(Object& object, HSQUIRRELVM vm)
	: ScriptComponent(object)
	, m_vm(vm)
{
	sq_resetobject(&m_scriptObject);
	for(auto i : Enumerator<EBuiltinFuncType>())
	{
		sq_resetobject(&m_builtinFunctions[i]);
	}

	// Call the constructor
	sq_pushroottable(m_vm);	// the 'this' parameter
	Sqrat::PushVar(m_vm, this);
	SQRESULT result = sq_call(m_vm, 2, true, true);
	if(SQ_SUCCEEDED(result))
	{
		sq_getstackobj(m_vm, -1, &m_scriptObject);
		sq_addref(m_vm, &m_scriptObject);

		sq_pop(m_vm, 1);	// pop instance

		// Cache built-in functions
		const char* builtinFunctionNames[] =
		{
			"Update",
			"Init",
			"Kill",
		};
		DESIRE_CHECK_ARRAY_SIZE(builtinFunctionNames, ScriptComponent::EBuiltinFuncType::Num);

		for(auto i : Enumerator<ScriptComponent::EBuiltinFuncType>())
		{
			sq_pushstring(m_vm, builtinFunctionNames[i], -1);
			result = sq_get(m_vm, -2);
			if(SQ_SUCCEEDED(result) && sq_gettype(m_vm, -1) == OT_CLOSURE)
			{
				sq_getstackobj(m_vm, -1, &m_builtinFunctions[i]);
				sq_addref(m_vm, &m_builtinFunctions[i]);

				sq_pop(m_vm, 1);
			}
		}
	}

	sq_pop(m_vm, 2);	// pop class and root table
}

SquirrelScriptComponent::~SquirrelScriptComponent()
{
	if(!sq_isnull(m_scriptObject))
	{
		sq_release(m_vm, &m_scriptObject);
		sq_resetobject(&m_scriptObject);
	}

	for(auto i : Enumerator<EBuiltinFuncType>())
	{
		if(!sq_isnull(m_builtinFunctions[i]))
		{
			sq_release(m_vm, &m_builtinFunctions[i]);
			sq_resetobject(&m_builtinFunctions[i]);
		}
	}
}

bool SquirrelScriptComponent::IsValid() const
{
	return !sq_isnull(m_scriptObject);
}

void SquirrelScriptComponent::CallByType(EBuiltinFuncType funcType)
{
	if(!IsValid() || sq_isnull(m_builtinFunctions[(size_t)funcType]))
	{
		return;
	}

	// Save the stack size before the call
	m_savedStackTop = sq_gettop(m_vm);

	sq_pushobject(m_vm, m_scriptObject);
	sq_pushobject(m_vm, m_builtinFunctions[(size_t)funcType]);

	sq_pushobject(m_vm, m_scriptObject);	// the 'this' parameter
	m_numFunctionCallArgs = 1;

	ExecuteFunctionCall();
}

SQInteger SquirrelScriptComponent::CallFromScript(HSQUIRRELVM vm)
{
	// argCount is the number of arguments to call the script function with (-2 because we need to exclude the instance and the function name)
	const SQInteger argCount = sq_gettop(vm) - 2;

	SquirrelScriptComponent* pScriptComponent = Sqrat::Var<SquirrelScriptComponent*>(vm, -2 - argCount).value;
	ASSERT(vm == pScriptComponent->m_vm);

	const SQChar* pFunctionName = nullptr;
	SQInteger size = 0;
	sq_getstringandsize(vm, -1 - argCount, &pFunctionName, &size);

	if(pScriptComponent->PrepareFunctionCall(String(pFunctionName, size)))
	{
		// Push the args
		const SQInteger argIdxOffsetFromTop = -argCount - 3;	// -3 because the object, the function and the 'this' parameter are pushed in PrepareFunctionCall()
		for(SQInteger i = 0; i < argCount; ++i)
		{
			sq_push(vm, argIdxOffsetFromTop);
			pScriptComponent->m_numFunctionCallArgs++;
		}

		pScriptComponent->ExecuteFunctionCall();
	}

	return 0;
}

bool SquirrelScriptComponent::PrepareFunctionCall(const String& functionName)
{
	if(!IsValid())
	{
		return false;
	}

	// Save the stack size before the call
	m_savedStackTop = sq_gettop(m_vm);

	sq_pushobject(m_vm, m_scriptObject);
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

	sq_pushobject(m_vm, m_scriptObject);	// the 'this' parameter
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
	sq_pushfloat(m_vm, static_cast<SQFloat>(arg));
	m_numFunctionCallArgs++;
	return true;
}

bool SquirrelScriptComponent::AddFunctionCallArg(bool arg)
{
	sq_pushbool(m_vm, static_cast<SQBool>(arg));
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
