#pragma once

#include "Engine/Scene/ISceneManager.h"
#include "Engine/Core/Container/Array.h"

#define MAX_FURSTUM_NORMAL	4

class Vector3;
class QuadTreeLeaf;
class DebugDraw;

class QuadTreeSceneManager : public ISceneManager
{
public:
	QuadTreeSceneManager();
	~QuadTreeSceneManager() override;

	void Add(RenderComponent* component, bool dynamic) override;
	void Remove(RenderComponent* component) override;

	void SetActiveCamera(Camera* camera) override;

	void Update() override;
	void Reset() override;

private:
	enum class EState : uint8_t
	{
		Inside,
		Outside,
		Intersect
	};

	void CalcFrustumNormalsFromCamera(Camera* camera, Vector3* normals, uint8_t& nNormal, float(&pointDotNormal)[MAX_FURSTUM_NORMAL], uint8_t(&aabbNPVertex)[MAX_FURSTUM_NORMAL][2]);

	void TestVisibleLeafs(uint8_t nNormal, const Vector3* normals, const float* pointDotNormal, const uint8_t(&aabbNPVertex)[MAX_FURSTUM_NORMAL][2]);
	void TestInvisibleLeafs(uint8_t nNormal, const Vector3* normals, const float* pointDotNormal, const uint8_t(&aabbNPVertex)[MAX_FURSTUM_NORMAL][2]);

	static void SetLeafsVisible_recursive(QuadTreeLeaf* ltmp, bool visible);
	static EState IsAabbVisible(const Vector3* points, uint8_t nNormal, const Vector3* normals, const float* pointDotNormal, const uint8_t(&aabbNPVertex)[MAX_FURSTUM_NORMAL][2]);

	QuadTreeLeaf* rootLeaf = nullptr;
	Camera* activeCamera = nullptr;

	RenderComponent** visibleDynamicComponents = nullptr;
	RenderComponent** invisibleDynamicComponents = nullptr;
	uint32_t numVisibleDynamicComponents = 0;
	uint32_t numInvisibleDynamicComponents = 0;
	uint32_t numAllocatedVisibleDynamicComponents;
	uint32_t numAllocatedInvisibleDynamicComponents;

	Array<QuadTreeLeaf*> visibleLeafList;
	Array<QuadTreeLeaf*> tmpLeafList;

	DebugDraw* debugDraw = nullptr;

	bool doInvisibleLeafTest = false;
	bool needToPlaceObjectsInsideQuadTree = false;
};
