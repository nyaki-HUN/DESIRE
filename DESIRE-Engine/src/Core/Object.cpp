#include "stdafx.h"
#include "Core/Object.h"
#include "Core/StrUtils.h"

Object::Object(const char *name)
	: objectID(0)
	, objectName(StrUtils::Duplicate(name))
{

}

Object::~Object()
{
	free(objectName);
}

void Object::AddComponent(IComponent *component)
{
	if(component == nullptr)
	{
		return;
	}

#if defined(DESIRE_DEBUG)
	const int typeID = component->GetTypeID();
	for(const auto& pair : components)
	{
		ASSERT(pair.first != typeID);
	}
#endif

	component->object = this;
	components.emplace_back(component->GetTypeID(), component);
}

void Object::SetObjectName(const char *name)
{
	free(objectName);
	objectName = StrUtils::Duplicate(name);
}

const char* Object::GetObjectName() const
{
	return objectName;
}

void Object::SetID(uint32_t id)
{
	objectID = id;
}

uint32_t Object::GetID() const
{
	return objectID;
}

IComponent* Object::GetComponentByTypeID(int typeID)
{
	for(const auto& pair : components)
	{
		if(pair.first == typeID)
		{
			return pair.second;
		}
	}
	return nullptr;
}

const IComponent* Object::GetComponentByTypeID(int typeID) const
{
	for(const auto& pair : components)
	{
		if(pair.first == typeID)
		{
			return pair.second;
		}
	}
	return nullptr;
}
