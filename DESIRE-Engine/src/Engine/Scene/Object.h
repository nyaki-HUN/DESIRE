#pragma once

#include <vector>
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

	template<class T, class... Args>
	T& AddComponent(Args&&... args)
	{
		return static_cast<T&>(AddComponent_Internal(std::make_unique<T>(*this, std::forward<Args>(args)...)));
	}

	// Removes and destroys a component
	void RemoveComponent(const Component *component);

	// Get the first component with the given typeID
	Component* GetComponentByTypeID(int typeID) const;

	// Get the first component with the given template type
	template<class T>
	T* GetComponent() const
	{
		return static_cast<T*>(GetComponentByTypeID(T::TYPE_ID));
	}

	// Get all components
	const std::vector<std::unique_ptr<Component>>& GetComponents() const;

	// Get all script components
	std::vector<ScriptComponent*> GetScriptComponents() const;

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
	Component& AddComponent_Internal(std::unique_ptr<Component>&& component);
	void RemoveChild_Internal(Object *child);
	void SetNewParent(Object *newParent);
	
	static void RefreshParentPointerInTransforms(Transform *firstTransform, size_t transformCount);

	std::vector<std::unique_ptr<Component>> components;
	Transform *transform = nullptr;
	std::unique_ptr<AABB> aabb;
	size_t numTransformsInHierarchy = 1;

	Object *parent = nullptr;
	std::vector<Object*> children;

	uint32_t objectID = 0;
	char *objectName = nullptr;
};
