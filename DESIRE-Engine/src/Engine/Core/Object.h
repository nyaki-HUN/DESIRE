#pragma once

#include "Engine/Core/Container/Array.h"
#include "Engine/Core/String/StackString.h"

class Component;
class Transform;

class Object
{
public:
	Object();
	~Object();

	void SetObjectName(const String& name);
	const String& GetObjectName() const;

	void SetActive(bool active);
	bool IsActiveSelf() const;
	bool IsActiveInHierarchy() const;

	void SetParent(Object* pNewParent);
	Object* GetParent() const;

	Object& CreateChildObject(const String& name);
	const Array<Object*>& GetChildren() const;
	Object* FindObjectByName(const String& name, bool isRecursiveSearch = false) const;
	bool HasObjectInParentHierarchy(const Object* pObject) const;

	template<class T, class... Args>
	T& AddComponent(Args&&... args)
	{
		return static_cast<T&>(AddComponent_Internal(std::make_unique<T>(*this, std::forward<Args>(args)...)));
	}

	// Removes and destroys a component
	void RemoveComponent(const Component* pComponent);

	// Get the component with the given typeID
	Component* GetComponentByTypeId(int32_t typeId) const;

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
	Component& AddComponent_Internal(std::unique_ptr<Component>&& component);
	void AddChild_Internal(Object* pChild);
	void RemoveChild_Internal(Object* pChild);

	static void RefreshParentPointerInTransforms(Transform* pFirstTransform, size_t transformCount);

	Array<std::unique_ptr<Component>> m_components;
	Transform* m_pTransform = nullptr;
	size_t m_numTransformsInHierarchy = 1;

	Object* m_pParent = nullptr;
	Array<Object*> m_children;

	bool m_isActive = true;

	StackString<kMaxObjectNameLength> m_objectName;
};
