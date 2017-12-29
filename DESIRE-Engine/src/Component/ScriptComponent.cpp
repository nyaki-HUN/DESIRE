#include "stdafx.h"
#include "Component/ScriptComponent.h"

ScriptComponent::ScriptComponent(Object& object)
	: Component(object)
{
	AddToObject();
}

ScriptComponent::~ScriptComponent()
{

}
