#include "Engine/stdafx.h"
#include "Engine/Script/ScriptSystem.h"
#include "Engine/Script/NativeScriptComponent.h"
#include "Engine/Core/Object.h"

ScriptSystem::ScriptSystem()
{
}

ScriptSystem::~ScriptSystem()
{
}

void ScriptSystem::RegisterScript(HashedString scriptName, Factory<IScript>::Func_t factory)
{
	ASSERT(factory != nullptr);

	scriptFactories.Insert(scriptName, factory);
}

ScriptComponent* ScriptSystem::CreateScriptComponentOnObject(Object& object, const String& scriptName)
{
	// Try to create as a native script 
	Factory<IScript>::Func_t* scriptFactory = scriptFactories.Find(HashedString::CreateFromString(scriptName));
	if(scriptFactory != nullptr)
	{
		NativeScriptComponent& scriptComponent = object.AddComponent<NativeScriptComponent>((*scriptFactory)());
		return &scriptComponent;
	}

	// Try to create from file
	return CreateScriptComponentOnObject_Internal(object, scriptName);
}

void ScriptSystem::OnScriptComponentCreated(ScriptComponent* component)
{
	scriptComponents.Add(component);
}

void ScriptSystem::OnScriptComponentDestroyed(ScriptComponent* component)
{
	scriptComponents.RemoveFast(component);
}

void ScriptSystem::Update()
{
	for(ScriptComponent* scriptComponent : scriptComponents)
	{
		scriptComponent->CallByType(ScriptComponent::EBuiltinFuncType::Update);
	}
}
