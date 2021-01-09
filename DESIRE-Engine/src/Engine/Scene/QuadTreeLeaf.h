#pragma once

#include "Engine/Core/Container/Array.h"
#include "Engine/Core/Math/AABB.h"

class RenderComponent;

class QuadTreeLeaf
{
public:
	QuadTreeLeaf(uint8_t level);
	~QuadTreeLeaf();

	void Add(RenderComponent* pRenderComponent);
	bool Remove(RenderComponent* pRenderComponent);

	void Init();

	QuadTreeLeaf* m_leafs[4] = {};
	Array<RenderComponent*> m_renderComponents;
	uint8_t m_level;

	AABB m_aabb;
	Vector3 m_aabbPoints[4];

private:
	static constexpr uint8_t kQuadTreeMaxLevel = 30;
	static constexpr size_t kQuadTreeMinObjectPerLeaf = 5;

	bool TryToInsert(RenderComponent* pRenderComponent);

	float m_maxSizeX = 0.0f;
	float m_maxSizeZ = 0.0f;
};
