#include "Engine/stdafx.h"
#include "Engine/Scene/QuadTreeLeaf.h"
#include "Engine/Render/RenderComponent.h"

QuadTreeLeaf::QuadTreeLeaf(uint8_t level)
	: level(level)
	, maxSizeX(0.0f)
	, maxSizeZ(0.0f)
{
	memset(leafs, 0, sizeof(leafs));

	for(int i = 0; i < 4; ++i)
	{
		aabbPoints[i] = Vector3::Zero();
	}
}

QuadTreeLeaf::~QuadTreeLeaf()
{
	for(QuadTreeLeaf* childLeaf : leafs)
	{
		delete childLeaf;
	}
}

void QuadTreeLeaf::Add(RenderComponent* component)
{
	aabb.AddAABB(component->GetAABB());
	renderComponents.Add(component);
}

bool QuadTreeLeaf::Remove(RenderComponent* component)
{
	if(renderComponents.RemoveFast(component))
	{
		return true;
	}

	for(QuadTreeLeaf* childLeaf : leafs)
	{
		if(childLeaf != nullptr && childLeaf->Remove(component))
		{
			return true;
		}
	}

	return false;
}

void QuadTreeLeaf::Init()
{
	constexpr float kMaxResizeFactor = 1.5f;

	// Calculate size limits
	const Vector3 size = aabb.GetSize();
	maxSizeX = size.GetX() * kMaxResizeFactor;
	maxSizeZ = size.GetZ() * kMaxResizeFactor;

	if(renderComponents.Size() <= kQuadTreeMinObjectPerLeaf)
	{
		return;
	}

	if(level > kQuadTreeMaxLevel)
	{
		return;
	}

	const Vector3 center = aabb.GetCenter();

	for(int i = 0; i < 4; i++)
	{
		leafs[i] = new QuadTreeLeaf(level + 1u);

/*		    X
		 -------
		| 2 | 3 |
		|-------| Z
		| 0 | 1 |
		 -------
*/
		switch(i)
		{
			case 0:	leafs[i]->aabb = AABB(aabb.GetMinEdge(), center); break;
			case 1:	leafs[i]->aabb = AABB(Vector3(center.GetX(), 0.0f, aabb.GetMinEdge().GetZ()), Vector3(aabb.GetMaxEdge().GetX(), 0.0f, center.GetZ())); break;
			case 2:	leafs[i]->aabb = AABB(Vector3(aabb.GetMinEdge().GetX(), 0.0f, center.GetZ()), Vector3(center.GetX(), 0.0f, aabb.GetMaxEdge().GetZ())); break;
			case 3:	leafs[i]->aabb = AABB(center, aabb.GetMaxEdge()); break;
		}
	}

	// Try to insert renderComponents into child leafs
	Array<RenderComponent*> renderComponentsToAdd;
	renderComponentsToAdd.Swap(renderComponents);
	for(RenderComponent* comp : renderComponentsToAdd)
	{
		bool addedToChild = false;
		for(QuadTreeLeaf* childLeaf : leafs)
		{
			if(childLeaf->TryToInsert(comp))
			{
				addedToChild = true;
				break;
			}
		}

		if(!addedToChild)
		{
			// Add back to the current leaf
			renderComponents.Add(comp);
		}
	}

	// Cache points from the AABB
	aabb.GetPoints2D(aabbPoints);

	// Init child leafs
	for(int i = 0; i < 4; i++)
	{
		if(leafs[i]->renderComponents.IsEmpty())
		{
			delete leafs[i];
			leafs[i] = nullptr;
		}
		else
		{
			leafs[i]->Init();
		}
	}
}

bool QuadTreeLeaf::TryToInsert(RenderComponent* component)
{
	const AABB& compAABB = component->GetAABB();
	if(!aabb.IsAABBFullyInside2D(compAABB))
	{
		// Don't insert if the center is otside of the box
		if(aabb.IsPointInside2D(compAABB.GetCenter()))
		{
			return false;
		}

		AABB newAABB = aabb;
		newAABB.AddAABB(compAABB);

		// Don't insert if it would make the box too big
		const Vector3 newSize = newAABB.GetSize();
		if(newSize.GetX() > maxSizeX || newSize.GetZ() > maxSizeZ)
		{
			return false;
		}

		aabb = newAABB;
	}

	renderComponents.Add(component);
	return true;
}
