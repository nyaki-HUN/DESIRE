#pragma once

#include "Engine/Scene/ISceneManager.h"
#include "Engine/Core/Container/Array.h"

class Vector3;
class QuadTreeLeaf;
class DebugDraw;

class QuadTreeSceneManager : public ISceneManager
{
public:
	QuadTreeSceneManager();
	~QuadTreeSceneManager() override;

	void Add(RenderComponent* pRenderComponent, bool dynamic) override;
	void Remove(RenderComponent* pRenderComponent) override;

	void SetActiveCamera(Camera* pCamera) override;

	void Update() override;
	void Reset() override;

private:
	static constexpr size_t kMaxFurstumNormal = 4;

	enum class EState : uint8_t
	{
		Inside,
		Outside,
		Intersect
	};

	void CalcFrustumNormalsFromCamera(Camera* pCamera, Vector3* pNormals, uint8_t& numNormals, float(&pointDotNormal)[kMaxFurstumNormal], uint8_t(&aabbNPVertex)[kMaxFurstumNormal][2]);

	void TestVisibleLeafs(uint8_t numNormals, const Vector3* pNormals, const float* pPointDotNormal, const uint8_t(&aabbNPVertex)[kMaxFurstumNormal][2]);
	void TestInvisibleLeafs(uint8_t numNormals, const Vector3* pNormals, const float* pPointDotNormal, const uint8_t(&aabbNPVertex)[kMaxFurstumNormal][2]);

	static void SetLeafsVisible_recursive(QuadTreeLeaf& leaf, bool visible);
	static EState IsAabbVisible(const Vector3* pPoints, uint8_t numNormals, const Vector3* pNormals, const float* pPointDotNormal, const uint8_t(&aabbNPVertex)[kMaxFurstumNormal][2]);

	QuadTreeLeaf* m_pRootLeaf = nullptr;
	Camera* m_pActiveCamera = nullptr;

	Array<RenderComponent*> m_visibleDynamicComponents;
	Array<RenderComponent*> m_invisibleDynamicComponents;

	Array<QuadTreeLeaf*> m_visibleLeafList;
	Array<QuadTreeLeaf*> m_tmpLeafList;

	DebugDraw* m_pDebugDraw = nullptr;

	bool m_doInvisibleLeafTest = false;
	bool m_needToPlaceObjectsInsideQuadTree = false;
};
