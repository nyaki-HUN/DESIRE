#include "stdafx_Squirrel.h"
#include "SquirrelScriptSystem.h"

#include "API/SquirrelScriptAPI.h"
#include "SquirrelScriptComponent.h"
#include "SquirrelCallbacks.h"

#include "Engine/Core/FS/FileSystem.h"
#include "Engine/Core/Object.h"
#include "Engine/Core/String/DynamicString.h"
#include "Engine/Core/String/StackString.h"

#include "Engine/Utils/Enumerator.h"

SquirrelScriptSystem::SquirrelScriptSystem()
{
	// Create a VM with initial stack size 1024 
	vm = sq_open(1024);
	sq_setprintfunc(vm, &SquirrelCallbacks::PrintCallback, &SquirrelCallbacks::ErrorCallback);

#if DESIRE_PUBLIC_BUILD
	sq_enabledebuginfo(vm, SQFalse);
#else
	sq_enabledebuginfo(vm, SQTrue);
	sq_setnativedebughook(vm, &SquirrelCallbacks::DebugHookCallback);
#endif

	sq_pushroottable(vm);

	// Set error handlers
	sq_newclosure(vm, &SquirrelCallbacks::RuntimeErrorHandler, 0);
	sq_seterrorhandler(vm);
	sq_setcompilererrorhandler(vm, &SquirrelCallbacks::CompilerErrorCallback);
	sq_pop(vm, 1);

	// Register Script API
	Sqrat::RootTable rootTable(vm);
	RegisterCoreAPI_Math_Squirrel(rootTable);
	RegisterCoreAPI_Squirrel(rootTable);
	RegisterInputAPI_Squirrel(rootTable);
	RegisterPhysicsAPI_Squirrel(rootTable);
	RegisterRenderAPI_Squirrel(rootTable);
	RegisterSoundAPI_Squirrel(rootTable);

	// ScriptComponent
	rootTable.Bind("ScriptComponent", Sqrat::DerivedClass<SquirrelScriptComponent, Component, Sqrat::NoConstructor<SquirrelScriptComponent>>(vm, "ScriptComponent")
		.SquirrelFunc("Call", &SquirrelScriptComponent::CallFromScript)
	);

	Sqrat::Class<Object, Sqrat::NoConstructor<Object>>(vm, "Object", false).Func<SquirrelScriptComponent* (Object::*)() const>("GetScriptComponent", &Object::GetComponent<SquirrelScriptComponent>);
}

SquirrelScriptSystem::~SquirrelScriptSystem()
{
	sq_close(vm);
}

ScriptComponent* SquirrelScriptSystem::CreateScriptComponentOnObject_Internal(Object& object, const String& scriptName)
{
	// Get factory function
	sq_pushroottable(vm);
	sq_pushstring(vm, scriptName.Str(), -1);
	SQRESULT result = sq_get(vm, -2);
	if(SQ_FAILED(result))
	{
		CompileScript(scriptName, vm);

		sq_pushstring(vm, scriptName.Str(), -1);
		result = sq_get(vm, -2);
		if(SQ_FAILED(result))
		{
			sq_pop(vm, 1);	// pop root table
			return nullptr;
		}
	}

	SquirrelScriptComponent* pScriptComponent = &object.AddComponent<SquirrelScriptComponent>(vm);

	// Call the constructor
	sq_pushroottable(vm);	// the 'this' parameter
	Sqrat::PushVar(vm, pScriptComponent);
	result = sq_call(vm, 2, true, true);
	if(SQ_SUCCEEDED(result))
	{
		sq_getstackobj(vm, -1, &pScriptComponent->scriptObject);
		sq_addref(vm, &pScriptComponent->scriptObject);

		sq_pop(vm, 1);	// pop instance

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
			sq_pushstring(vm, builtinFunctionNames[i], -1);
			result = sq_get(vm, -2);
			if(SQ_SUCCEEDED(result) && sq_gettype(vm, -1) == OT_CLOSURE)
			{
				sq_getstackobj(vm, -1, &pScriptComponent->builtinFunctions[i]);
				sq_addref(vm, &pScriptComponent->builtinFunctions[i]);

				sq_pop(vm, 1);
			}
		}
	}
	else
	{
		object.RemoveComponent(pScriptComponent);
		pScriptComponent = nullptr;
	}

	sq_pop(vm, 2);	// pop class and root table

	return pScriptComponent;
}

void SquirrelScriptSystem::CompileScript(const String& scriptName, HSQUIRRELVM vm)
{
	const StackString<DESIRE_MAX_PATH_LEN> filename = StackString<DESIRE_MAX_PATH_LEN>::Format("data/scripts/%s.nut", scriptName.Str());
	DynamicString data = FileSystem::Get()->LoadTextFile(filename);
	DynamicString scriptSrc;
	scriptSrc.Sprintf(
		"class %s"
		"{"
		"	self = null;"
		"	constructor(component) { self = component; }"
		, scriptName.Str());
	scriptSrc += data;
	scriptSrc += "}";

	SQRESULT result = sq_compilebuffer(vm, scriptSrc.Str(), (SQInteger)scriptSrc.Length(), scriptName.Str(), SQTrue);
	if(SQ_FAILED(result))
	{
		LOG_ERROR("Could not compile script: %s", filename.Str());
		return;
	}

	HSQOBJECT scriptModule;
	sq_getstackobj(vm, -1, &scriptModule);
	ASSERT(sq_isclosure(scriptModule));

	// Execute script module
	sq_pushroottable(vm);
	result = sq_call(vm, 1 /*root table*/, false, true);
	ASSERT(SQ_SUCCEEDED(result));

	sq_pop(vm, 1);	// pop scriptModule
}
