#pragma once

#include "Core/math/AABB.h"

#include <vector>

class SceneNodeComponent;

class QuadTreeLeaf
{
public:
	QuadTreeLeaf(uint8_t level);
	~QuadTreeLeaf();

	void AddComponent(SceneNodeComponent *component);
	bool RemoveComponent(SceneNodeComponent *component);

	void Init();

	QuadTreeLeaf *leafs[4];
	std::vector<SceneNodeComponent*> components;
	uint8_t level;

	AABB aabb;
	Vector3 aabbPoints[4];

private:
	static const int QUAD_TREE_MAX_LEVEL = 30;
	static const int QUAD_TREE_MIN_OBJECT_PER_LEAF = 5;

	bool TryToInsertComponent(SceneNodeComponent *component);

	float maxSizeX;
	float maxSizeZ;
};
