#pragma once

#include "Engine/Core/Container/Array.h"

#include <memory>

class Component;
class ScriptComponent;
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

	void SetParent(Object *newParent);
	Object* CreateChildObject(const char *name);

	template<class T, class... Args>
	T& AddComponent(Args&&... args)
	{
		return static_cast<T&>(AddComponent_Internal(std::make_unique<T>(*this, std::forward<Args>(args)...)));
	}

	// Removes and destroys a component
	void RemoveComponent(const Component *component);

	// Get the component with the given typeID
	Component* GetComponentByTypeID(int typeID) const;

	// Get the component with the given template type
	template<class T>
	T* GetComponent() const
	{
		return static_cast<T*>(GetComponentByTypeID(T::kTypeID));
	}

	// Get all components
	const Array<std::unique_ptr<Component>>& GetComponents() const;

	// Get all script components
	Array<ScriptComponent*> GetScriptComponents() const;

	Transform& GetTransform() const;
	const AABB& GetAABB() const;
	Object* GetParent() const;
	const Array<Object*>& GetChildren() const;

	bool HasObjectInParentHierarchy(const Object *obj) const;

	void UpdateAllTransformsInHierarchy();

private:
	Component& AddComponent_Internal(std::unique_ptr<Component>&& component);
	void AddChild_Internal(Object *child);
	void RemoveChild_Internal(Object *child);

	static void RefreshParentPointerInTransforms(Transform *firstTransform, size_t transformCount);

	Array<std::unique_ptr<Component>> components;
	Transform *transform = nullptr;
	std::unique_ptr<AABB> aabb;
	size_t numTransformsInHierarchy = 1;

	Object *parent = nullptr;
	Array<Object*> children;

	uint32_t objectID = 0;
	char *objectName = nullptr;
};
