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
	m_vm = sq_open(1024);
	sq_setprintfunc(m_vm, &SquirrelCallbacks::PrintCallback, &SquirrelCallbacks::ErrorCallback);

#if DESIRE_PUBLIC_BUILD
	sq_enabledebuginfo(m_vm, SQFalse);
#else
	sq_enabledebuginfo(m_vm, SQTrue);
	sq_setnativedebughook(m_vm, &SquirrelCallbacks::DebugHookCallback);
#endif

	sq_pushroottable(m_vm);

	// Set error handlers
	sq_newclosure(m_vm, &SquirrelCallbacks::RuntimeErrorHandler, 0);
	sq_seterrorhandler(m_vm);
	sq_setcompilererrorhandler(m_vm, &SquirrelCallbacks::CompilerErrorCallback);
	sq_pop(m_vm, 1);

	// Register Script API
	Sqrat::RootTable rootTable(m_vm);
	RegisterCoreAPI_Math_Squirrel(rootTable);
	RegisterCoreAPI_Squirrel(rootTable);
	RegisterInputAPI_Squirrel(rootTable);
	RegisterPhysicsAPI_Squirrel(rootTable);
	RegisterRenderAPI_Squirrel(rootTable);
	RegisterSoundAPI_Squirrel(rootTable);

	// ScriptComponent
	rootTable.Bind("ScriptComponent", Sqrat::DerivedClass<SquirrelScriptComponent, Component, Sqrat::NoConstructor<SquirrelScriptComponent>>(m_vm, "ScriptComponent")
		.SquirrelFunc("Call", &SquirrelScriptComponent::CallFromScript)
	);

	Sqrat::Class<Object, Sqrat::NoConstructor<Object>>(m_vm, "Object", false).Func<SquirrelScriptComponent* (Object::*)() const>("GetScriptComponent", &Object::GetComponent<SquirrelScriptComponent>);
}

SquirrelScriptSystem::~SquirrelScriptSystem()
{
	sq_close(m_vm);
}

ScriptComponent* SquirrelScriptSystem::CreateScriptComponentOnObject_Internal(Object& object, const String& scriptName)
{
	// Get factory function
	sq_pushroottable(m_vm);
	sq_pushstring(m_vm, scriptName.Str(), -1);
	SQRESULT result = sq_get(m_vm, -2);
	if(SQ_FAILED(result))
	{
		CompileScript(scriptName, m_vm);

		sq_pushstring(m_vm, scriptName.Str(), -1);
		result = sq_get(m_vm, -2);
		if(SQ_FAILED(result))
		{
			sq_pop(m_vm, 1);	// pop root table
			return nullptr;
		}
	}

	SquirrelScriptComponent& scriptComponent = object.AddComponent<SquirrelScriptComponent>(m_vm);
	if(!scriptComponent.IsValid())
	{
		object.RemoveComponent(&scriptComponent);
		return nullptr;
	}

	return &scriptComponent;
}

void SquirrelScriptSystem::CompileScript(const String& scriptName, HSQUIRRELVM vm)
{
	const StackString<DESIRE_MAX_PATH_LEN> filename = StackString<DESIRE_MAX_PATH_LEN>::Format("data/scripts/%s.nut", scriptName.Str());
	DynamicString data = FileSystem::Get().LoadTextFile(filename);
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
