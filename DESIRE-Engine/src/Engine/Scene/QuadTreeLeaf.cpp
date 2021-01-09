#include "Engine/stdafx.h"
#include "Engine/Scene/QuadTreeLeaf.h"

#include "Engine/Render/RenderComponent.h"

QuadTreeLeaf::QuadTreeLeaf(uint8_t level)
	: m_level(level)
{
	for(int32_t i = 0; i < 4; ++i)
	{
		m_aabbPoints[i] = Vector3::Zero();
	}
}

QuadTreeLeaf::~QuadTreeLeaf()
{
	for(QuadTreeLeaf* pChildLeaf : m_leafs)
	{
		delete pChildLeaf;
	}
}

void QuadTreeLeaf::Add(RenderComponent* pRenderComponent)
{
	m_aabb.AddAABB(pRenderComponent->GetAABB());
	m_renderComponents.Add(pRenderComponent);
}

bool QuadTreeLeaf::Remove(RenderComponent* pRenderComponent)
{
	if(m_renderComponents.RemoveFast(pRenderComponent))
	{
		return true;
	}

	for(QuadTreeLeaf* pChildLeaf : m_leafs)
	{
		if(pChildLeaf != nullptr && pChildLeaf->Remove(pRenderComponent))
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
	const Vector3 size = m_aabb.GetSize();
	m_maxSizeX = size.GetX() * kMaxResizeFactor;
	m_maxSizeZ = size.GetZ() * kMaxResizeFactor;

	if(m_renderComponents.Size() <= kQuadTreeMinObjectPerLeaf)
	{
		return;
	}

	if(m_level > kQuadTreeMaxLevel)
	{
		return;
	}

	const Vector3 center = m_aabb.GetCenter();

	for(size_t i = 0; i < 4; i++)
	{
		m_leafs[i] = new QuadTreeLeaf(m_level + 1u);

/*		    X
		 -------
		| 2 | 3 |
		|-------| Z
		| 0 | 1 |
		 -------
*/
		switch(i)
		{
			case 0:	m_leafs[i]->m_aabb = AABB(m_aabb.GetMinEdge(), center); break;
			case 1:	m_leafs[i]->m_aabb = AABB(Vector3(center.GetX(), 0.0f, m_aabb.GetMinEdge().GetZ()), Vector3(m_aabb.GetMaxEdge().GetX(), 0.0f, center.GetZ())); break;
			case 2:	m_leafs[i]->m_aabb = AABB(Vector3(m_aabb.GetMinEdge().GetX(), 0.0f, center.GetZ()), Vector3(center.GetX(), 0.0f, m_aabb.GetMaxEdge().GetZ())); break;
			case 3:	m_leafs[i]->m_aabb = AABB(center, m_aabb.GetMaxEdge()); break;
		}
	}

	// Try to insert renderComponents into child leafs
	Array<RenderComponent*> renderComponentsToAdd;
	renderComponentsToAdd.Swap(m_renderComponents);
	for(RenderComponent* pRenderComponent : renderComponentsToAdd)
	{
		bool addedToChild = false;
		for(QuadTreeLeaf* pChildLeaf : m_leafs)
		{
			if(pChildLeaf->TryToInsert(pRenderComponent))
			{
				addedToChild = true;
				break;
			}
		}

		if(!addedToChild)
		{
			// Add back to the current leaf
			m_renderComponents.Add(pRenderComponent);
		}
	}

	// Cache points from the AABB
	m_aabb.GetPoints2D(m_aabbPoints);

	// Init child leafs
	for(size_t i = 0; i < 4; i++)
	{
		if(m_leafs[i]->m_renderComponents.IsEmpty())
		{
			delete m_leafs[i];
			m_leafs[i] = nullptr;
		}
		else
		{
			m_leafs[i]->Init();
		}
	}
}

bool QuadTreeLeaf::TryToInsert(RenderComponent* pRenderComponent)
{
	const AABB& compAABB = pRenderComponent->GetAABB();
	if(!m_aabb.IsAABBFullyInside2D(compAABB))
	{
		// Don't insert if the center is otside of the box
		if(m_aabb.IsPointInside2D(compAABB.GetCenter()))
		{
			return false;
		}

		AABB newAABB = m_aabb;
		newAABB.AddAABB(compAABB);

		// Don't insert if it would make the box too big
		const Vector3 newSize = newAABB.GetSize();
		if(newSize.GetX() > m_maxSizeX || newSize.GetZ() > m_maxSizeZ)
		{
			return false;
		}

		m_aabb = newAABB;
	}

	m_renderComponents.Add(pRenderComponent);
	return true;
}
