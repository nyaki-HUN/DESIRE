#include "Engine/stdafx.h"
#include "Engine/Script/ScriptComponent.h"

#include "Engine/Script/ScriptSystem.h"

ScriptComponent::ScriptComponent(Object& object)
	: Component(object)
{
	Modules::ScriptSystem->OnScriptComponentCreated(this);
}

ScriptComponent::~ScriptComponent()
{
	Modules::ScriptSystem->OnScriptComponentDestroyed(this);
}

void ScriptComponent::CloneTo(Object& otherObject) const
{
	DESIRE_UNUSED(otherObject);
	DESIRE_TODO("Implement CloneTo");
//	Modules::ScriptSystem->CreateScriptComponentOnObject(otherObject, );
//	otherComponent.SetEnabled(IsEnabled());
}
