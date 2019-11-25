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

void Component::Destroy()
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

Object& Component::GetObject() const
{
	return object;
}
