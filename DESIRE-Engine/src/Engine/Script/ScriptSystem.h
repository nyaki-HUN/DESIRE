#pragma once

#include "Engine/Core/Container/Array.h"
#include "Engine/Core/Container/HashedStringMap.h"
#include "Engine/Core/Factory.h"

class IScript;
class ScriptComponent;
class Object;

class ScriptSystem
{
public:
	typedef Factory<IScript, ScriptComponent&> ScriptFactory;

	ScriptSystem();
	virtual ~ScriptSystem();

	void Update();

	void RegisterScript(HashedString scriptName, ScriptFactory::Func_t factory);
	ScriptComponent* CreateScriptComponentOnObject(Object& object, const String& scriptName);

	void OnScriptComponentCreated(ScriptComponent* pScriptComponent);
	void OnScriptComponentDestroyed(ScriptComponent* pScriptComponent);

private:
	virtual ScriptComponent* CreateScriptComponentOnObject_Internal(Object& object, const String& scriptName) = 0;

	Array<ScriptComponent*> m_scriptComponents;
	HashedStringMap<ScriptFactory::Func_t> m_scriptFactories;
};

#define REGISTER_NATIVE_SCRIPT(SCRIPT)	Modules::ScriptSystem->RegisterScript(HashedString(#SCRIPT), &ScriptSystem::ScriptFactory::Create<SCRIPT>)
