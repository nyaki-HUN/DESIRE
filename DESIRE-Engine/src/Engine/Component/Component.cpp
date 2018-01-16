#include "Engine/stdafx.h"
#include "Engine/Component/Component.h"
#include "Engine/Scene/Object.h"

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
