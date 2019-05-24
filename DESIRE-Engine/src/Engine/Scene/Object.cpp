#include "Engine/stdafx.h"
#include "Engine/Scene/Object.h"
#include "Engine/Scene/Transform.h"
#include "Engine/Core/Component.h"
#include "Engine/Core/math/AABB.h"

#define MAX_TRANSFORMS	10000
static Transform preallocatedTransforms[MAX_TRANSFORMS];
static size_t numTransforms = 0;

Object::Object(const String& name)
	: aabb(std::make_unique<AABB>())
	, objectName(name)
{
	SetTransform();
}

Object::Object(const DynamicString&& name)
	: aabb(std::make_unique<AABB>())
	, objectName(std::move(name))
{
	SetTransform();
}

Object::~Object()
{
	// If the owner of the transform is set to nullptr we are called from a parent object's destructor and no need to call SetParent()
	if(transform->owner != nullptr)
	{
		SetParent(nullptr);
		numTransforms -= numTransformsInHierarchy;
	}

	for(Object *child : children)
	{
		child->transform->owner = nullptr;
		delete child;
	}

	transform->parent = nullptr;
	transform->owner = nullptr;
	transform->ResetToIdentity();
}

void Object::SetObjectName(const String& name)
{
	objectName = name;
}

const String& Object::GetObjectName() const
{
	return objectName;
}

void Object::SetActive(bool active)
{

}

void Object::SetVisible(bool visible)
{

}

void Object::SetParent(Object *newParent)
{
	if(parent == newParent)
	{
		return;
	}

	if(parent != nullptr)
	{
		parent->RemoveChild_Internal(this);
	}

	Transform *oldTransform = transform;
	if(newParent != nullptr)
	{
		transform = newParent->transform + newParent->numTransformsInHierarchy;

		// If the parent transform will be moved, we need to apply correction
		if(newParent->transform > oldTransform)
		{
			transform -= numTransformsInHierarchy;
		}

		newParent->AddChild_Internal(this);
	}
	else
	{
		transform = &preallocatedTransforms[numTransforms];
	}

	ptrdiff_t numToMove = oldTransform - transform;
	if(numToMove != 0)
	{
		Transform *savedTransforms = &preallocatedTransforms[numTransforms];
		ASSERT(numTransformsInHierarchy <= DESIRE_ASIZEOF(preallocatedTransforms) - numTransforms);
		memcpy(savedTransforms, oldTransform, numTransformsInHierarchy * sizeof(Transform));

		Transform *movedTransformDst = nullptr;
		Transform *movedTransformSrc = nullptr;
		if(numToMove < 0)
		{
			numToMove = std::abs(numToMove);
			// Move data to the left in the array (our transforms will be placed after it)
			movedTransformDst = oldTransform;
			movedTransformSrc = oldTransform + numTransformsInHierarchy;
		}
		else
		{
			// Move data to the right in the array (our transforms will be placed before it)
			movedTransformDst = transform + numTransformsInHierarchy;
			movedTransformSrc = transform;
		}

		memmove(movedTransformDst, movedTransformSrc, numToMove * sizeof(Transform));
		RefreshParentPointerInTransforms(movedTransformDst, numToMove);

		memcpy(transform, savedTransforms, numTransformsInHierarchy * sizeof(Transform));
		RefreshParentPointerInTransforms(transform, numTransformsInHierarchy);
	}

	transform->flags |= Transform::WORLD_MATRIX_DIRTY;
	parent = newParent;
}

Object* Object::CreateChildObject(const String& name)
{
	Object *obj = new Object(name);
	obj->SetParent(this);
	return obj;
}

Object* Object::CreateChildObject(DynamicString&& name)
{
	Object *obj = new Object(std::move(name));
	obj->SetParent(this);
	return obj;
}

void Object::RemoveComponent(const Component *component)
{
	const size_t idx = components.SpecializedFind([component](const std::unique_ptr<Component>& comp)
	{
		return (comp.get() == component);
	});

	if(idx != SIZE_MAX)
	{
		components.RemoveAt(idx);
	}
}

Component* Object::GetComponentByTypeId(int typeId) const
{
	// Binary find
	auto it = std::lower_bound(components.begin(), components.end(), typeId, [](const std::unique_ptr<Component>& component, int id)
	{
		return (component->GetTypeId() < id);
	});
	return (it != components.end() && !(typeId < (*it)->GetTypeId())) ? it->get() : nullptr;
}

const Array<std::unique_ptr<Component>>& Object::GetComponents() const
{
	return components;
}

Transform& Object::GetTransform() const
{
	return *transform;
}

const AABB& Object::GetAABB() const
{
	return *aabb;
}

Object* Object::GetParent() const
{
	return parent;
}

const Array<Object*>& Object::GetChildren() const
{
	return children;
}

bool Object::HasObjectInParentHierarchy(const Object *obj) const
{
	const Object *otmp = parent;
	while(otmp != nullptr)
	{
		if(otmp == obj)
		{
			return true;
		}

		otmp = otmp->parent;
	}

	return false;
}

void Object::UpdateAllTransformsInHierarchy()
{
	Transform *transformTmp = transform;
	for(size_t i = 0; i < numTransformsInHierarchy; i++)
	{
		transformTmp->UpdateWorldMatrix();
		transformTmp++;
	}
}

Component& Object::AddComponent_Internal(std::unique_ptr<Component> component)
{
	std::unique_ptr<Component>& addedComponent = components.BinaryFindOrInsert(std::move(component), [](const std::unique_ptr<Component>& a, const std::unique_ptr<Component>& b)
	{
		return (a->GetTypeId() < b->GetTypeId());
	});
	return *(addedComponent.get());
}

void Object::AddChild_Internal(Object *child)
{
	Object *obj = this;
	do
	{
		obj->numTransformsInHierarchy += child->numTransformsInHierarchy;
		obj = obj->parent;
	} while(obj != nullptr);

	children.Add(child);

	child->transform->parent = transform;
}

void Object::RemoveChild_Internal(Object *child)
{
	Object *obj = this;
	do
	{
		ASSERT(obj->numTransformsInHierarchy > child->numTransformsInHierarchy);
		obj->numTransformsInHierarchy -= child->numTransformsInHierarchy;
		obj = obj->parent;
	} while(obj != nullptr);

	children.Remove(child);
	child->transform->parent = nullptr;
}

void Object::SetTransform()
{
	ASSERT(numTransforms < MAX_TRANSFORMS);
	transform = &preallocatedTransforms[numTransforms++];
	transform->owner = this;
}

void Object::RefreshParentPointerInTransforms(Transform *firstTransform, size_t transformCount)
{
	Transform *transformTmp = firstTransform;
	for(size_t i = 0; i < transformCount; ++i)
	{
		transformTmp->owner->transform = transformTmp;
		if(transformTmp->owner->parent != nullptr)
		{
			transformTmp->parent = &transformTmp->owner->parent->GetTransform();
		}

		transformTmp++;
	}
}
