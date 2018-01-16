#include "Engine/stdafx.h"
#include "Engine/Component/ScriptComponent.h"
#include "Engine/Script/ScriptSystem.h"

ScriptComponent::ScriptComponent(Object& object)
	: Component(object)
{
	ScriptSystem::Get()->OnScriptComponentCreate(this);
}

ScriptComponent::~ScriptComponent()
{
	ScriptSystem::Get()->OnScriptComponentDestroy(this);
}
