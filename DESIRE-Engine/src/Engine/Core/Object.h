#pragma once

#include "Engine/Core/Container/Array.h"
#include "Engine/Core/String/StackString.h"

#include <memory>

class Component;
class Transform;

class Object
{
public:
	Object(const String& name = "Object");
	~Object();

	void SetObjectName(const String& name);
	const String& GetObjectName() const;

	void SetActive(bool active);
	bool IsActiveSelf() const;
	bool IsActiveInHierarchy() const;

	void SetParent(Object* newParent);
	Object* GetParent() const;

	Object* CreateChildObject(const String& name);
	const Array<Object*>& GetChildren() const;
	bool HasObjectInParentHierarchy(const Object* obj) const;

	template<class T, class... Args>
	T& AddComponent(Args&&... args)
	{
		return static_cast<T&>(AddComponent_Internal(std::make_unique<T>(*this, std::forward<Args>(args)...)));
	}

	// Removes and destroys a component
	void RemoveComponent(const Component* component);

	// Get the component with the given typeID
	Component* GetComponentByTypeId(int typeId) const;

	// Get the component with the given template type
	template<class T>
	T* GetComponent() const
	{
		return static_cast<T*>(GetComponentByTypeId(T::kTypeId));
	}

	// Get all components
	const Array<std::unique_ptr<Component>>& GetComponents() const;

	Transform& GetTransform() const;

	void MarkAllChildrenTransformDirty();

	static constexpr size_t kMaxObjectNameLength = 32;

private:
	Component& AddComponent_Internal(std::unique_ptr<Component> component);
	void AddChild_Internal(Object* child);
	void RemoveChild_Internal(Object* child);

	void SetTransform();

	static void RefreshParentPointerInTransforms(Transform* firstTransform, size_t transformCount);

	Array<std::unique_ptr<Component>> components;
	Transform* transform = nullptr;
	size_t numTransformsInHierarchy = 1;

	Object* parent = nullptr;
	Array<Object*> children;

	bool isActive = true;

	StackString<kMaxObjectNameLength> objectName;
};
