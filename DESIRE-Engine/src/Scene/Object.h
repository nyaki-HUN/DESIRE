#pragma once

#include <vector>
#include <memory>

class Component;
class Transform;
class AABB;

class Object
{
	friend class Component;

public:
	Object(const char *name = nullptr);
	~Object();

	const char* GetObjectName() const;
	void SetObjectName(const char *name);

	uint32_t GetID() const;
	void SetID(uint32_t id);

	void SetActive(bool active);
	void SetVisible(bool visible);

	Component* GetComponentByTypeID(int typeID);
	const Component* GetComponentByTypeID(int typeID) const;

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

	const std::vector<Component*>& GetComponents() const;

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
	
	static void RefreshParentPointerInTransforms(Transform *firstTransform, size_t transformCount);

	std::vector<Component*> components;
	Transform *transform = nullptr;
	std::unique_ptr<AABB> aabb;
	size_t numTransformsInHierarchy = 1;

	Object *parent = nullptr;
	std::vector<Object*> children;

	uint32_t objectID = 0;
	char *objectName = nullptr;
};
