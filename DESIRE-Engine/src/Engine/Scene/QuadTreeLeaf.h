#pragma once

#include "Engine/Core/Container/Array.h"
#include "Engine/Core/math/AABB.h"

class Object;

class QuadTreeLeaf
{
public:
	QuadTreeLeaf(uint8_t level);
	~QuadTreeLeaf();

	void AddObject(Object *obj);
	bool RemoveObject(Object *obj);

	void Init();

	QuadTreeLeaf *leafs[4];
	Array<Object*> objects;
	uint8_t level;

	AABB aabb;
	Vector3 aabbPoints[4];

private:
	static constexpr int kQuadTreeMaxLevel = 30;
	static constexpr int kQuadTreeMinObjectPerLeaf = 5;

	bool TryToInsertObject(Object *obj);

	float maxSizeX;
	float maxSizeZ;
};
