#include "Engine/stdafx.h"
#include "Engine/Component/ScriptComponent.h"
#include "Engine/Core/Modules.h"
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
//	Modules::ScriptSystem->CreateScriptComponentOnObject(otherObject, );
}
