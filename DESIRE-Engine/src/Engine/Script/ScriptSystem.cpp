#include "Engine/stdafx.h"
#include "Engine/Script/ScriptSystem.h"

#include "Engine/Core/Object.h"

#include "Engine/Script/NativeScriptComponent.h"

ScriptSystem::ScriptSystem()
{
}

ScriptSystem::~ScriptSystem()
{
}

void ScriptSystem::RegisterScript(HashedString scriptName, ScriptFactory::Func_t factory)
{
	ASSERT(factory != nullptr);

	m_scriptFactories.Insert(scriptName, factory);
}

ScriptComponent* ScriptSystem::CreateScriptComponentOnObject(Object& object, const String& scriptName)
{
	// Try to create as a native script 
	ScriptFactory::Func_t* pScriptFactoryFunc = m_scriptFactories.Find(scriptName);
	if(pScriptFactoryFunc)
	{
		NativeScriptComponent& scriptComponent = object.AddComponent<NativeScriptComponent>(*pScriptFactoryFunc);
		return &scriptComponent;
	}

	// Try to create from file
	return CreateScriptComponentOnObject_Internal(object, scriptName);
}

void ScriptSystem::OnScriptComponentCreated(ScriptComponent* pScriptComponent)
{
	m_scriptComponents.Add(pScriptComponent);
}

void ScriptSystem::OnScriptComponentDestroyed(ScriptComponent* pScriptComponent)
{
	m_scriptComponents.RemoveFast(pScriptComponent);
}

void ScriptSystem::Update()
{
	for(ScriptComponent* pScriptComponent : m_scriptComponents)
	{
		pScriptComponent->CallByType(ScriptComponent::EBuiltinFuncType::Update);
	}
}
