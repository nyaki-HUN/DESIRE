#include "stdafx.h"
#include "Scene/Object.h"
#include "Scene/Transform.h"
#include "Core/math/AABB.h"
#include "Core/StrUtils.h"

#define MAX_TRANSFORMS	10000
static Transform preallocatedTransforms[MAX_TRANSFORMS];
static size_t numTransforms;

Object::Object(const char *name)
	: transform(nullptr)
	, aabb(std::make_unique<AABB>())
	, numTransformsInHierarchy(1)
	, parent(nullptr)
	, objectID(0)
	, objectName(nullptr)
{
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
	// If the owner of the transform is set to nullptr we are called from a parent object's destructor and we can skip the following block
	if(transform->owner != nullptr)
	{
		Remove();
		numTransforms -= numTransformsInHierarchy;

		for(Object *child : children)
		{
			child->transform->owner = nullptr;
			delete child;
		}
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

void Object::AddComponent(IComponent *component)
{
	if(component == nullptr)
	{
		return;
	}

#if defined(DESIRE_DEBUG)
	const int typeID = component->GetTypeID();
	for(const auto& pair : components)
	{
		ASSERT(pair.first != typeID);
	}
#endif

	component->object = this;
	components.emplace_back(component->GetTypeID(), component);
}

IComponent* Object::GetComponentByTypeID(int typeID)
{
	for(const auto& pair : components)
	{
		if(pair.first == typeID)
		{
			return pair.second;
		}
	}
	return nullptr;
}

const IComponent* Object::GetComponentByTypeID(int typeID) const
{
	for(const auto& pair : components)
	{
		if(pair.first == typeID)
		{
			return pair.second;
		}
	}
	return nullptr;
}

const std::vector<std::pair<int, IComponent*>>& Object::GetComponents() const
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

	children.erase(std::remove(children.begin(), children.end(), child), children.end());
}

void Object::SetNewParent(Object *newParent)
{
	Transform *oldTransform = transform;
	transform = (newParent != nullptr) ? newParent->transform + newParent->numTransformsInHierarchy : &preallocatedTransforms[numTransforms];
	ptrdiff_t numToMove = oldTransform - transform;
	if(numToMove != 0)
	{
		Transform *savedTransforms = &preallocatedTransforms[numTransforms];
		ASSERT(numTransformsInHierarchy <= DESIRE_ASIZEOF(preallocatedTransforms) - numTransforms);
		memcpy(savedTransforms, oldTransform, numTransformsInHierarchy * sizeof(Transform));

		Transform *movedTransformTmp = nullptr;
		if(numToMove < 0)
		{
			transform -= numTransformsInHierarchy;
			numToMove = transform - oldTransform;
			ASSERT(numToMove > 0);
			// Move data backward in array (our transforms will move forward)
			movedTransformTmp = oldTransform;
			memmove(oldTransform, oldTransform + numTransformsInHierarchy, numToMove * sizeof(Transform));
		}
		else
		{
			// Move data forward in array (our transforms will move backward)
			movedTransformTmp = transform + numTransformsInHierarchy;
			memmove(movedTransformTmp, transform, numToMove * sizeof(Transform));
		}

		// Refresh parentWorldMatrix pointers in moved transforms
		for(; numToMove > 0; numToMove--)
		{
			movedTransformTmp->owner->transform = movedTransformTmp;
			if(movedTransformTmp->owner->parent != nullptr)
			{
				movedTransformTmp->parentWorldMatrix = &movedTransformTmp->owner->parent->GetTransform().GetWorldMatrix();
			}
			movedTransformTmp++;
		}

		memcpy(transform, savedTransforms, numTransformsInHierarchy * sizeof(Transform));

		// Refresh parentWorldMatrix pointers in own hierarchy
		movedTransformTmp = transform;
		for(size_t i = 0; i < numTransformsInHierarchy; i++)
		{
			movedTransformTmp->owner->transform = movedTransformTmp;
			if(movedTransformTmp->owner->parent != nullptr)
			{
				movedTransformTmp->parentWorldMatrix = &movedTransformTmp->owner->parent->GetTransform().GetWorldMatrix();
			}
			movedTransformTmp++;
		}
	}

	transform->parentWorldMatrix = (newParent != nullptr) ? &newParent->transform->worldMatrix : nullptr;
	transform->flags |= Transform::WORLD_MATRIX_DIRTY;

	parent = newParent;
}
