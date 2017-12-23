#include "stdafx.h"
#include "Script/IScriptSystem.h"
#include "Script/NativeScriptComponent.h"

IScriptSystem::IScriptSystem()
{

}

IScriptSystem::~IScriptSystem()
{

}

void IScriptSystem::RegisterScript(HashedString scriptName, ScriptFactory_t factory)
{
	ASSERT(factory != nullptr);

	scriptFactories.Insert(scriptName, factory);
}

ScriptComponent* IScriptSystem::CreateScriptComponent(const char *scriptName)
{
	ScriptComponent *scriptComponent = nullptr;

	// Try to create as a native script 
	HashedString scriptNameHash = HashedString::CreateFromDynamicString(scriptName);
	ScriptFactory_t *scriptFactory = scriptFactories.Find(scriptNameHash);
	if(scriptFactory != nullptr)
	{
		scriptComponent = new NativeScriptComponent((*scriptFactory)());
	}
	else
	{
		// Try to create from file
		scriptComponent = CreateScriptComponent_Internal(scriptName);
	}

	if(scriptComponent != nullptr)
	{
		components.push_back(scriptComponent);
	}

	return scriptComponent;
}

void IScriptSystem::Update()
{
	for(ScriptComponent *scriptComponent : components)
	{
		scriptComponent->CallByType(ScriptComponent::EBuiltinFuncType::UPDATE);
	}
}
