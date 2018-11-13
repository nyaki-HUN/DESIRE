#include "Engine/stdafx.h"
#include "Engine/Script/ScriptComponent.h"
#include "Engine/Script/ScriptSystem.h"
#include "Engine/Core/Modules.h"

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
//	Modules::ScriptSystem->CreateScriptComponentOnObject(otherObject, );
}
