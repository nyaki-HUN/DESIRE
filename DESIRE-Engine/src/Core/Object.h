#pragma once

#include "Component/IComponent.h"

#include <vector>

class Object
{
public:
	Object(const char *name = nullptr);
	~Object();

	void AddComponent(IComponent *component);

	const char* GetObjectName() const;
	void SetObjectName(const char *name);

	uint32_t GetID() const;
	void SetID(uint32_t id);

	IComponent* GetComponentByTypeID(int typeID);
	const IComponent* GetComponentByTypeID(int typeID) const;

	template<class T>
	T* GetComponent()
	{
		return static_cast<T*>(GetComponentByTypeID(T::TYPE_ID));
	}

	template<class T>
	const T* GetComponent() const
	{
		return static_cast<T*>(GetComponentByTypeID(T::TYPE_ID));
	}

private:
	std::vector<std::pair<int, IComponent*>> components;
	uint32_t objectID;
	char *objectName;
};
