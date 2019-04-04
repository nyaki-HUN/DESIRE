#pragma once

#include "Engine/Core/Factory.h"
#include "Engine/Core/Container/Array.h"
#include "Engine/Core/HashedStringMap.h"

class IScript;
class ScriptComponent;
class Object;

class ScriptSystem
{
public:
	ScriptSystem();
	virtual ~ScriptSystem();

	void Update();

	void RegisterScript(HashedString scriptName, Factory<IScript>::Func_t factory);
	ScriptComponent* CreateScriptComponentOnObject(Object& object, const char *scriptName);

	void OnScriptComponentCreated(ScriptComponent *component);
	void OnScriptComponentDestroyed(ScriptComponent *component);

private:
	virtual ScriptComponent* CreateScriptComponentOnObject_Internal(Object& object, const char *scriptName) = 0;

	Array<ScriptComponent*> scriptComponents;
	HashedStringMap<Factory<IScript>::Func_t> scriptFactories;
};

#define REGISTER_NATIVE_SCRIPT(SCRIPT)	Modules::ScriptSystem->RegisterScript(HashedString(#SCRIPT), &Factory<IScript>::Create<SCRIPT>)
