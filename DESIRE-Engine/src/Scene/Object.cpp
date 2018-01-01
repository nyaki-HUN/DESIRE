#include "stdafx.h"
#include "Scene/Object.h"
#include "Scene/Transform.h"
#include "Component/Component.h"
#include "Core/math/AABB.h"
#include "Core/StrUtils.h"

#define MAX_TRANSFORMS	10000
static Transform preallocatedTransforms[MAX_TRANSFORMS];
static size_t numTransforms = 0;

Object::Object(const char *name)
{
	aabb = std::make_unique<AABB>();

	if(name == nullptr)
	{
		objectName = StrUtils::Duplicate("Object");
	}
	else
	{
		objectName = StrUtils::Duplicate(name);
	}

	ASSERT(numTransforms < MAX_TRANSFORMS);
	transform = &preallocatedTransforms[numTransforms++];
	transform->parentWorldMatrix = nullptr;
	transform->owner = this;
	transform->ResetToIdentity();
}

Object::~Object()
{
	// If the owner of the transform is set to nullptr we are called from a parent object's destructor and no need to call Remove()
	if(transform->owner != nullptr)
	{
		Remove();
		numTransforms -= numTransformsInHierarchy;
	}

	for(Object *child : children)
	{
		child->transform->owner = nullptr;
		delete child;
	}

	transform->parentWorldMatrix = nullptr;
	transform->owner = nullptr;

	free(objectName);
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

void Object::SetActive(bool active)
{

}

void Object::SetVisible(bool visible)
{

}

Component* Object::GetComponentByTypeID(int typeID)
{
	for(Component *component : components)
	{
		if(component->GetTypeID() == typeID)
		{
			return component;
		}
	}

	return nullptr;
}

const Component* Object::GetComponentByTypeID(int typeID) const
{
	for(const Component *component : components)
	{
		if(component->GetTypeID() == typeID)
		{
			return component;
		}
	}

	return nullptr;
}

const std::vector<Component*>& Object::GetComponents() const
{
	return components;
}

void Object::Remove()
{
	if(parent != nullptr)
	{
		parent->RemoveChild(this);
	}
}

void Object::AddChild(Object *child)
{
	ASSERT(child != nullptr);

	if(child->parent == this)
	{
		// Already added
		return;
	}

	if(child->parent != nullptr)
	{
		child->parent->RemoveChild_Internal(child);
	}

	child->SetNewParent(this);

	Object *obj = this;
	do
	{
		obj->numTransformsInHierarchy += child->numTransformsInHierarchy;
		obj = obj->parent;
	} while(obj != nullptr);

	children.push_back(child);
}

void Object::RemoveChild(Object *child)
{
	ASSERT(child != nullptr);

	if(child->parent != this)
	{
		return;
	}

	RemoveChild_Internal(child);

	child->SetNewParent(nullptr);
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

const std::vector<Object*>& Object::GetChildren() const
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

void Object::RemoveChild_Internal(Object *child)
{
	Object *obj = this;
	do
	{
		ASSERT(obj->numTransformsInHierarchy > child->numTransformsInHierarchy);
		obj->numTransformsInHierarchy -= child->numTransformsInHierarchy;
		obj = obj->parent;
	} while(obj != nullptr);

	auto it = std::find(children.begin(), children.end(), child);
	if(it != children.end())
	{
		children.erase(it);
	}
}

void Object::SetNewParent(Object *newParent)
{
	Transform *oldTransform = transform;
	if(newParent != nullptr)
	{
		transform = newParent->transform + newParent->numTransformsInHierarchy;

		// If the parent transform will be moved, we need to apply correction
		if(newParent->transform > oldTransform)
		{
			transform -= numTransformsInHierarchy;
		}
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
		RefreshParentWorldMatrixPointersInTransforms(movedTransformDst, numToMove);

		memcpy(transform, savedTransforms, numTransformsInHierarchy * sizeof(Transform));
		RefreshParentWorldMatrixPointersInTransforms(transform, numTransformsInHierarchy);
	}

	transform->parentWorldMatrix = (newParent != nullptr) ? &newParent->transform->worldMatrix : nullptr;
	transform->flags |= Transform::WORLD_MATRIX_DIRTY;

	parent = newParent;
}

void Object::RefreshParentWorldMatrixPointersInTransforms(Transform *firstTransform, size_t transformCount)
{
	Transform *transformTmp = firstTransform;
	for(size_t i = 0; i < transformCount; ++i)
	{
		transformTmp->owner->transform = transformTmp;
		if(transformTmp->owner->parent != nullptr)
		{
			transformTmp->parentWorldMatrix = &transformTmp->owner->parent->GetTransform().GetWorldMatrix();
		}

		transformTmp++;
	}
}
