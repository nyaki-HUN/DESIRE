#include "stdafx.h"
#include "Component/ScriptComponent.h"
#include "Script/ScriptSystem.h"

ScriptComponent::ScriptComponent(Object& object)
	: Component(object)
{
	ScriptSystem::Get()->OnScriptComponentCreate(this);
}

ScriptComponent::~ScriptComponent()
{
	ScriptSystem::Get()->OnScriptComponentDestroy(this);
}
