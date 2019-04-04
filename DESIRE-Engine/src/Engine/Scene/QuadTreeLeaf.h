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
	static const int QUAD_TREE_MAX_LEVEL = 30;
	static const int QUAD_TREE_MIN_OBJECT_PER_LEAF = 5;

	bool TryToInsertObject(Object *obj);

	float maxSizeX;
	float maxSizeZ;
};
