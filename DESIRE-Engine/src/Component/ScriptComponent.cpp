#include "stdafx.h"
#include "Component/ScriptComponent.h"
#include "Script/IScriptSystem.h"

ScriptComponent::ScriptComponent(Object& object)
	: Component(object)
{
	IScriptSystem::Get()->OnScriptComponentCreate(this);
}

ScriptComponent::~ScriptComponent()
{
	IScriptSystem::Get()->OnScriptComponentDestroy(this);
}
