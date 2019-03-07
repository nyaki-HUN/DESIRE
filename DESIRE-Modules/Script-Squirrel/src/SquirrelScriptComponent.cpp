#include "SquirrelScriptComponent.h"

#include "Engine/Core/assert.h"
#include "Engine/Core/Log.h"
#include "Engine/Core/String.h"
#include "Engine/Utils/Enumerator.h"

#include "sqrat/sqratTypes.h"

SquirrelScriptComponent::SquirrelScriptComponent(Object& object, HSQUIRRELVM vm)
	: ScriptComponent(object)
	, vm(vm)
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
		sq_release(vm, &scriptObject);
		sq_resetobject(&scriptObject);
	}

	for(auto i : Enumerator<EBuiltinFuncType>())
	{
		if(!sq_isnull(builtinFunctions[i]))
		{
			sq_release(vm, &builtinFunctions[i]);
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
	savedStackTop = sq_gettop(vm);

	sq_pushobject(vm, scriptObject);
	sq_pushobject(vm, builtinFunctions[(size_t)funcType]);

	sq_pushobject(vm, scriptObject);	// the 'this' parameter
	numFunctionCallArgs = 1;

	ExecuteFunctionCall();
}

SQInteger SquirrelScriptComponent::CallFromScript(HSQUIRRELVM vm)
{
	// argCount is the number of arguments to call the script function with (-2 because we need to exclude the instance and the function name)
	const SQInteger argCount = sq_gettop(vm) - 2;

	SquirrelScriptComponent *scriptComp = Sqrat::Var<SquirrelScriptComponent*>(vm, -2 - argCount).value;
	ASSERT(vm == scriptComp->vm);

	const char *functionName;
	sq_getstring(vm, -1 - argCount, &functionName);

	if(scriptComp->PrepareFunctionCall(functionName))
	{
		// Push the args
		const SQInteger argIdxOffsetFromTop = -argCount - 3;	// -3 because the object, the function and the 'this' parameter are pushed in PrepareFunctionCall()
		for(SQInteger i = 0; i < argCount; ++i)
		{
			sq_push(vm, argIdxOffsetFromTop);
			scriptComp->numFunctionCallArgs++;
		}

		scriptComp->ExecuteFunctionCall();
	}

	return 0;
}

bool SquirrelScriptComponent::PrepareFunctionCall(const char *functionName)
{
	ASSERT(functionName != nullptr);

	// Save the stack size before the call
	savedStackTop = sq_gettop(vm);

	sq_pushobject(vm, scriptObject);
	sq_pushstring(vm, functionName, -1);
	if(SQ_FAILED(sq_get(vm, -2)))
	{
		sq_settop(vm, savedStackTop);
		return false;
	}

	// Verify that the function exists
	const SQObjectType type = sq_gettype(vm, -1);
	if(type != OT_CLOSURE && type != OT_NATIVECLOSURE)
	{
		sq_settop(vm, savedStackTop);
		return false;
	}

	sq_pushobject(vm, scriptObject);	// the 'this' parameter
	numFunctionCallArgs = 1;
	return true;
}

void SquirrelScriptComponent::ExecuteFunctionCall()
{
	const SQRESULT result = sq_call(vm, numFunctionCallArgs, SQFalse, SQTrue);
	// Restore the original stack size
	sq_settop(vm, savedStackTop);
	
	if(SQ_FAILED(result))
	{
		LOG_ERROR("Execution error");
	}
}

bool SquirrelScriptComponent::AddFunctionCallArg(int arg)
{
	sq_pushinteger(vm, arg);
	numFunctionCallArgs++;
	return true;
}

bool SquirrelScriptComponent::AddFunctionCallArg(float arg)
{
	sq_pushfloat(vm, arg);
	numFunctionCallArgs++;
	return true;
}

bool SquirrelScriptComponent::AddFunctionCallArg(double arg)
{
	sq_pushfloat(vm, (SQFloat)arg);
	numFunctionCallArgs++;
	return true;
}

bool SquirrelScriptComponent::AddFunctionCallArg(bool arg)
{
	sq_pushbool(vm, (SQBool)arg);
	numFunctionCallArgs++;
	return true;
}

bool SquirrelScriptComponent::AddFunctionCallArg(void *arg)
{
	ASSERT(arg != nullptr);
	sq_pushuserpointer(vm, arg);
	numFunctionCallArgs++;
	return true;
}

bool SquirrelScriptComponent::AddFunctionCallArg(const String& arg)
{
	sq_pushstring(vm, arg.Str(), (SQInteger)arg.Length());
	numFunctionCallArgs++;
	return true;
}
