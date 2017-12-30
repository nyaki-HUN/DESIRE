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

void Component::AddToObject()
{
	ASSERT(object.GetComponentByTypeID(GetTypeID()) == nullptr);
	object.components.emplace_back(GetTypeID(), this);
}
