#include "Engine/stdafx.h"
#include "Engine/Component/ScriptComponent.h"
#include "Engine/Core/Modules.h"
#include "Engine/Script/ScriptSystem.h"

ScriptComponent::ScriptComponent(Object& object)
	: Component(object)
{
	Modules::ScriptSystem->OnScriptComponentCreate(this);
}

ScriptComponent::~ScriptComponent()
{
	Modules::ScriptSystem->OnScriptComponentDestroy(this);
}
