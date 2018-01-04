#include "stdafx.h"
#include "Component/Component.h"
#include "Scene/Object.h"

Component::Component(Object& object)
	: object(object)
{
	// Add to object
	object.components.push_back(this);
}

Component::~Component()
{
	// Remove from object
	auto it = std::find(object.components.begin(), object.components.end(), this);
	if(it != object.components.end())
	{
		object.components.erase(it);
	}
}
