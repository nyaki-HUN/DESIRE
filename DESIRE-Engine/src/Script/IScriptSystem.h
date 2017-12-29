#pragma once

#include "Core/Singleton.h"
#include "Core/HashedStringMap.h"

#include <vector>
#include <memory>

class IScript;
class ScriptComponent;
class Object;

class IScriptSystem
{
	DESIRE_DECLARE_SINGLETON_INTERFACE(IScriptSystem)

public:
	typedef std::unique_ptr<IScript>(*ScriptFactory_t)();

	void Update();

	void RegisterScript(HashedString scriptName, ScriptFactory_t factory);
	ScriptComponent* CreateScriptComponent(Object *object, const char *scriptName);

private:
	virtual ScriptComponent* CreateScriptComponent_Internal(Object& object, const char *scriptName) = 0;

	std::vector<ScriptComponent*> components;
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

#define REGISTER_NATIVE_SCRIPT(SCRIPT)	IScriptSystem::Get()->RegisterScript(HashedString(#SCRIPT), &NativeScriptFactory<SCRIPT>::Create)
