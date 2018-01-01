#include "stdafx.h"
#include "Scene/QuadTreeLeaf.h"
#include "Scene/Object.h"

QuadTreeLeaf::QuadTreeLeaf(uint8_t level)
	: level(level)
	, maxSizeX(0.0f)
	, maxSizeZ(0.0f)
{
	memset(leafs, 0, sizeof(leafs));

	for(int i = 0; i < 4; ++i)
	{
		aabbPoints[i] = Vector3(0.0f);
	}
}

QuadTreeLeaf::~QuadTreeLeaf()
{
	for(QuadTreeLeaf *childLeaf : leafs)
	{
		delete childLeaf;
	}
}

void QuadTreeLeaf::AddObject(Object *obj)
{
	aabb.AddAABB(obj->GetAABB());
	objects.push_back(obj);
}

bool QuadTreeLeaf::RemoveObject(Object *obj)
{
	auto it = std::find(objects.begin(), objects.end(), obj);
	if(it != objects.end())
	{
		objects.erase(it);
		return true;
	}

	for(QuadTreeLeaf *childLeaf : leafs)
	{
		if(childLeaf != nullptr && childLeaf->RemoveObject(obj))
		{
			return true;
		}
	}

	return false;
}

void QuadTreeLeaf::Init()
{
	// Calculate size limits
	const Vector3 size = aabb.GetSize();
	static const float MAX_RESIZE_FACTOR = 1.5f;
	maxSizeX = size.GetX() * MAX_RESIZE_FACTOR;
	maxSizeZ = size.GetZ() * MAX_RESIZE_FACTOR;

	if(objects.size() <= QUAD_TREE_MIN_OBJECT_PER_LEAF)
	{
		return;
	}

	if(level > QUAD_TREE_MAX_LEVEL)
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

	// Try to insert objects into child leafs
	std::vector<Object*> objectsToAdd;
	objectsToAdd.swap(objects);
	for(Object *comp : objectsToAdd)
	{
		bool addedToChild = false;
		for(QuadTreeLeaf *childLeaf : leafs)
		{
			if(childLeaf->TryToInsertObject(comp))
			{
				addedToChild = true;
				break;
			}
		}

		if(!addedToChild)
		{
			// Add back to the current leaf
			objects.push_back(comp);
		}
	}

	// Cache points from the AABB
	aabb.GetPoints2D(aabbPoints);

	// Init child leafs
	for(int i = 0; i < 4; i++)
	{
		if(leafs[i]->objects.empty())
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

bool QuadTreeLeaf::TryToInsertObject(Object *obj)
{
	const AABB& compAABB = obj->GetAABB();
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

	objects.push_back(obj);
	return true;
}
