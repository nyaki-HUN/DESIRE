#pragma once

#include "Engine/Core/Container/Array.h"
#include "Engine/Core/math/AABB.h"

class RenderComponent;

class QuadTreeLeaf
{
public:
	QuadTreeLeaf(uint8_t level);
	~QuadTreeLeaf();

	void Add(RenderComponent* component);
	bool Remove(RenderComponent* component);

	void Init();

	QuadTreeLeaf* leafs[4];
	Array<RenderComponent*> renderComponents;
	uint8_t level;

	AABB aabb;
	Vector3 aabbPoints[4];

private:
	static constexpr int kQuadTreeMaxLevel = 30;
	static constexpr int kQuadTreeMinObjectPerLeaf = 5;

	bool TryToInsert(RenderComponent* component);

	float maxSizeX;
	float maxSizeZ;
};
