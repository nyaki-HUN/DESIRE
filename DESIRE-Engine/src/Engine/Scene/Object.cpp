#include "Engine/stdafx.h"
#include "Engine/Scene/Object.h"
#include "Engine/Scene/Transform.h"
#include "Engine/Script/ScriptComponent.h"
#include "Engine/Core/Component.h"
#include "Engine/Core/math/AABB.h"
#include "Engine/Core/String/StrUtils.h"

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
	transform->owner = this;
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

Object* Object::CreateChildObject(const char *name)
{
	Object *obj = new Object(name);
	obj->SetParent(this);
	return obj;
}

void Object::RemoveComponent(const Component *component)
{
	for(size_t i = 0; i < components.Size(); ++i)
	{
		if(components[i].get() == component)
		{
			components.Remove(i);
			return;
		}
	}
}

Component* Object::GetComponentByTypeID(int typeID) const
{
	// Binary find
	auto it = std::lower_bound(components.begin(), components.end(), typeID, [](const std::unique_ptr<Component>& component, int id)
	{
		return (component->GetTypeID() < id);
	});
	return (it != components.end() && !(typeID < (*it)->GetTypeID())) ? it->get() : nullptr;
}

const Array<std::unique_ptr<Component>>& Object::GetComponents() const
{
	return components;
}

Array<ScriptComponent*> Object::GetScriptComponents() const
{
	Array<ScriptComponent*> scriptComponents;
	for(const std::unique_ptr<Component>& component : components)
	{
		if(component->GetTypeID() == ScriptComponent::TYPE_ID)
		{
			scriptComponents.Add(static_cast<ScriptComponent*>(component.get()));
		}
	}

	return scriptComponents;
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

Component& Object::AddComponent_Internal(std::unique_ptr<Component>&& component)
{
	std::unique_ptr<Component>& addedComponent = components.BinaryFindOrInsert(std::move(component), [](const std::unique_ptr<Component>& a, const std::unique_ptr<Component>& b)
	{
		return (a->GetTypeID() < b->GetTypeID());
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
