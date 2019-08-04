#pragma once

#include "Engine/Core/Container/Array.h"
#include "Engine/Core/String/DynamicString.h"

#include <memory>

class Component;
class Transform;

class Object
{
public:
	Object(const String& name = "Object");
	Object(const DynamicString&& name);
	~Object();

	const String& GetObjectName() const;
	void SetObjectName(const String& name);

	void SetActive(bool active);

	void SetParent(Object* newParent);
	Object* CreateChildObject(const String& name);
	Object* CreateChildObject(DynamicString&& name);

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
	Object* GetParent() const;
	const Array<Object*>& GetChildren() const;

	bool HasObjectInParentHierarchy(const Object* obj) const;

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

	bool isActive = false;

	DynamicString objectName;
};
