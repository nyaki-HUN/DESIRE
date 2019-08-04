#include "Engine/stdafx.h"
#include "Engine/Core/Component.h"
#include "Engine/Core/Object.h"

Component::Component(Object& object)
	: object(object)
{

}

Component::~Component()
{

}

void Component::Destroy() const
{
	object.RemoveComponent(this);
}

void Component::SetEnabled(bool value)
{
	enabled = value;
}

bool Component::IsEnabled() const
{
	return enabled;
}
