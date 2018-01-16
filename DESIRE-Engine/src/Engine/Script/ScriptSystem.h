#pragma once

#include "Core/Singleton.h"
#include "Core/HashedStringMap.h"

#include <vector>
#include <memory>

class IScript;
class ScriptComponent;
class Object;

class ScriptSystem
{
	DESIRE_DECLARE_SINGLETON(ScriptSystem)

public:
	typedef std::unique_ptr<IScript>(*ScriptFactory_t)();

	void Update();

	void RegisterScript(HashedString scriptName, ScriptFactory_t factory);
	void CreateScriptComponentOnObject(Object& object, const char *scriptName);

	void OnScriptComponentCreate(ScriptComponent *component);
	void OnScriptComponentDestroy(ScriptComponent *component);

private:
	virtual void CreateScriptComponentOnObject_Internal(Object& object, const char *scriptName) = 0;

	std::vector<ScriptComponent*> scriptComponents;
	HashedStringMap<ScriptFactory_t> scriptFactories;
};

// Helper class for registering native scripts
template<class T>
class NativeScriptFactory
{
public:
	inline static std::unique_ptr<IScript> Create()
	{
		return std::make_unique<T>();
	}
};

#define REGISTER_NATIVE_SCRIPT(SCRIPT)	ScriptSystem::Get()->RegisterScript(HashedString(#SCRIPT), &NativeScriptFactory<SCRIPT>::Create)
