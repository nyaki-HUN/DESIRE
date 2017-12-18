#pragma once

#include "Component/IComponent.h"

#include <vector>
#include <memory>

class Transform;
class AABB;

class Object
{
public:
	Object(const char *name = nullptr);
	~Object();

	const char* GetObjectName() const;
	void SetObjectName(const char *name);

	uint32_t GetID() const;
	void SetID(uint32_t id);

	void SetActive(bool active);
	void SetVisible(bool visible);

	void AddComponent(IComponent *component);

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

	const std::vector<std::pair<int, IComponent*>>& GetComponents() const;

	// Remove from scene hierarchy
	void Remove();

	// Add the child to this object (if the child already has a parent, it is removed first)
	void AddChild(Object *child);

	// Remove the child from this object
	void RemoveChild(Object *child);

	Transform& GetTransform() const;
	const AABB& GetAABB() const;
	Object* GetParent() const;
	const std::vector<Object*>& GetChildren() const;

	bool HasObjectInParentHierarchy(const Object *obj) const;

	void UpdateAllTransformsInHierarchy();

private:
	void RemoveChild_Internal(Object *child);
	void SetNewParent(Object *newParent);

	std::vector<std::pair<int, IComponent*>> components;
	Transform *transform;
	std::unique_ptr<AABB> aabb;
	size_t numTransformsInHierarchy;

	Object *parent;
	std::vector<Object*> children;

	uint32_t objectID;
	char *objectName;
};
