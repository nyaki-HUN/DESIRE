#include "Engine/stdafx.h"
#include "Engine/Core/Component.h"

#include "Engine/Core/Object.h"

Component::Component(Object& object)
	: m_object(object)
{
}

Component::~Component()
{
}

void Component::Destroy()
{
	m_object.RemoveComponent(this);
}

void Component::SetEnabled(bool value)
{
	m_enabled = value;
}

bool Component::IsEnabled() const
{
	return m_enabled;
}

Object& Component::GetObject() const
{
	return m_object;
}
