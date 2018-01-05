#include "stdafx.h"
#include "Component/Component.h"
#include "Scene/Object.h"

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
