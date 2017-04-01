#include "stdafx.h"
#include "Component/SceneNodeComponent.h"
#include "Core/math/AABB.h"
#include "Scene/Transform.h"

#define MAX_TRANSFORMS	10000
Transform preallocatedTransforms[MAX_TRANSFORMS];
uint32_t numTransforms;

SceneNodeComponent::SceneNodeComponent()
	: transform(nullptr)
	, aabb(nullptr)
	, numTransformsInHierarchy(1)
	, parent(nullptr)
	, flags(0)
{
	ASSERT(numTransforms < MAX_TRANSFORMS);
	transform = &preallocatedTransforms[numTransforms];
	numTransforms++;

	transform->owner = this;
	transform->ResetToIdentity();

	aabb = new AABB();
}

SceneNodeComponent::~SceneNodeComponent()
{
	Remove();
}

void SceneNodeComponent::Remove()
{
	if(parent != nullptr)
	{
		parent->RemoveChild(this);
	}
}

void SceneNodeComponent::SetVisible(bool visible)
{

}

void SceneNodeComponent::AddChild(SceneNodeComponent *child)
{
	ASSERT(child != this);

	if(child->parent == this)
	{
		return;
	}

	if(child->parent != nullptr)
	{
		SceneNodeComponent *node = child->parent;
		do
		{
			ASSERT(node->numTransformsInHierarchy > child->numTransformsInHierarchy);
			node->numTransformsInHierarchy -= child->numTransformsInHierarchy;
			node = node->parent;
		} while(node != nullptr);

		child->parent->children.erase(std::remove(child->parent->children.begin(), child->parent->children.end(), child), child->parent->children.end());
	}

	child->SetNewParent(this);

	SceneNodeComponent *node = this;
	do
	{
		node->numTransformsInHierarchy += child->numTransformsInHierarchy;
		node = node->parent;
	} while(node != nullptr);

	children.push_back(child);
}

void SceneNodeComponent::RemoveChild(SceneNodeComponent *child)
{
	ASSERT(child->GetParent() == this);

	SceneNodeComponent *node = this;
	do
	{
		ASSERT(node->numTransformsInHierarchy > child->numTransformsInHierarchy);
		node->numTransformsInHierarchy -= child->numTransformsInHierarchy;
		node = node->parent;
	} while(node != nullptr);

	children.erase(std::remove(children.begin(), children.end(), child), children.end());

	child->SetNewParent(nullptr);
}

Transform& SceneNodeComponent::GetTransform()
{
	return *transform;
}

const AABB& SceneNodeComponent::GetAABB() const
{
	return *aabb;
}

SceneNodeComponent* SceneNodeComponent::GetParent()
{
	return parent;
}

const std::vector<SceneNodeComponent*>& SceneNodeComponent::GetChildren() const
{
	return children;
}

void SceneNodeComponent::UpdateAllTransformsInHierarchy()
{
	Transform *transformTmp = transform;
	for(size_t i = 0; i < numTransformsInHierarchy; i++)
	{
		transformTmp->UpdateWorldMatrix();
		transformTmp++;
	}
}

void SceneNodeComponent::SetNewParent(SceneNodeComponent *newParent)
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

		// Refresh transform pointers in moved transforms
		for(; numToMove > 0; numToMove--)
		{
			movedTransformTmp->owner->transform = movedTransformTmp;
			if(movedTransformTmp->owner->parent != nullptr)
			{
				movedTransformTmp->parent = &movedTransformTmp->owner->parent->GetTransform();
			}
			movedTransformTmp++;
		}

		memcpy(transform, savedTransforms, numTransformsInHierarchy * sizeof(Transform));

		// Refresh transform pointers in own hierarchy
		movedTransformTmp = transform;
		for(size_t i = 0; i < numTransformsInHierarchy; i++)
		{
			movedTransformTmp->owner->transform = movedTransformTmp;
			if(movedTransformTmp->owner->parent != nullptr)
			{
				movedTransformTmp->parent = &movedTransformTmp->owner->parent->GetTransform();
			}
			movedTransformTmp++;
		}
	}

	transform->SetParent((newParent != nullptr) ? newParent->transform : nullptr);
	parent = newParent;
}
