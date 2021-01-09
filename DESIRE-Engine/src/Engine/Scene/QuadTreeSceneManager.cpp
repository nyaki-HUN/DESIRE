#include "Engine/stdafx.h"
#include "Engine/Scene/QuadTreeSceneManager.h"

#include "Engine/Core/Math/AABB.h"

#include "Engine/Render/Camera.h"
#include "Engine/Render/DebugDraw.h"
#include "Engine/Render/RenderComponent.h"

#include "Engine/Scene/QuadTreeLeaf.h"

QuadTreeSceneManager::QuadTreeSceneManager()
	: m_pRootLeaf(new QuadTreeLeaf(0))
{
	m_visibleDynamicComponents.Reserve(512);
	m_invisibleDynamicComponents.Reserve(512);

	m_visibleLeafList.Reserve(256);
	m_tmpLeafList.Reserve(256);
}

QuadTreeSceneManager::~QuadTreeSceneManager()
{
	delete m_pRootLeaf;
}

void QuadTreeSceneManager::Add(RenderComponent* pRenderComponent, bool dynamic)
{
	if(dynamic)
	{
		m_visibleDynamicComponents.Add(pRenderComponent);
	}
	else
	{
		m_pRootLeaf->Add(pRenderComponent);
		m_needToPlaceObjectsInsideQuadTree = true;
	}

	pRenderComponent->SetVisible(true);
}

void QuadTreeSceneManager::Remove(RenderComponent* pRenderComponent)
{
	if(m_visibleDynamicComponents.Remove(pRenderComponent))
	{
		return;
	}

	if(m_invisibleDynamicComponents.Remove(pRenderComponent))
	{
		return;
	}

	m_pRootLeaf->Remove(pRenderComponent);
}

void QuadTreeSceneManager::SetActiveCamera(Camera* pCamera)
{
	m_pActiveCamera = pCamera;

	// Set all dynamic objects visible
	for(RenderComponent* pRenderComponent : m_invisibleDynamicComponents)
	{
		pRenderComponent->SetVisible(true);
		m_visibleDynamicComponents.Add(pRenderComponent);
	}
	m_invisibleDynamicComponents.Clear();

	// Set all leafs visible
	SetLeafsVisible_recursive(*m_pRootLeaf, true);

	m_visibleLeafList.Clear();
	m_doInvisibleLeafTest = true;
}

void QuadTreeSceneManager::Update()
{
	if(m_needToPlaceObjectsInsideQuadTree)
	{
		m_pRootLeaf->Init();
		m_needToPlaceObjectsInsideQuadTree = false;
	}

	if(m_pDebugDraw != nullptr && m_doInvisibleLeafTest)
	{
		m_pDebugDraw->Reset();
	}

	uint8_t numNormals;
	Vector3 normals[kMaxFurstumNormal];
	float pPointDotNormal[kMaxFurstumNormal];
	uint8_t aabbNPVertex[kMaxFurstumNormal][2];
	CalcFrustumNormalsFromCamera(m_pActiveCamera, normals, numNormals, pPointDotNormal, aabbNPVertex);

	if(m_doInvisibleLeafTest)
	{
		TestInvisibleLeafs(numNormals, normals, pPointDotNormal, aabbNPVertex);
	}
	else
	{
		TestVisibleLeafs(numNormals, normals, pPointDotNormal, aabbNPVertex);
	}

	m_doInvisibleLeafTest = !m_doInvisibleLeafTest;
}

void QuadTreeSceneManager::Reset()
{
	delete m_pRootLeaf;
	m_pRootLeaf = new QuadTreeLeaf(0);

	m_visibleDynamicComponents.Clear();
	m_invisibleDynamicComponents.Clear();

	m_visibleLeafList.Clear();

	m_doInvisibleLeafTest = true;
}

void QuadTreeSceneManager::CalcFrustumNormalsFromCamera(Camera* pCamera, Vector3* pNormals, uint8_t& numNormals, float(&pointDotNormal)[kMaxFurstumNormal], uint8_t(&aabbNPVertex)[kMaxFurstumNormal][2])
{
	numNormals = 0;

	Vector3 points[8];
	pCamera->CalculateFrustum(points);

	if(m_pDebugDraw != nullptr)
	{
		const Vector3 color(0.5f, 0.0f, 1.0f);

		// Far plane points
		m_pDebugDraw->AddLine(points[0], points[1], color);
		m_pDebugDraw->AddLine(points[1], points[3], color);
		m_pDebugDraw->AddLine(points[2], points[0], color);
		m_pDebugDraw->AddLine(points[3], points[2], color);
		// Near plane points
		m_pDebugDraw->AddLine(points[4], points[5], color);
		m_pDebugDraw->AddLine(points[5], points[7], color);
		m_pDebugDraw->AddLine(points[6], points[4], color);
		m_pDebugDraw->AddLine(points[7], points[6], color);

		m_pDebugDraw->AddLine(points[0], points[4], color);
		m_pDebugDraw->AddLine(points[1], points[5], color);
		m_pDebugDraw->AddLine(points[2], points[6], color);
		m_pDebugDraw->AddLine(points[3], points[7], color);
	}

	for(uint8_t i = 0; i < 8; i++)
	{
		points[i].SetY(0.0f);
	}

	const Vector3 up(0.0f, 1.0f, 0.0f);
	Vector3 vec, n;

	for(uint8_t i = 0; i < 4; i++)
	{
		vec = points[i] - points[i + 4];
		n = up.Cross(vec).Normalized();

		uint8_t j = 0;
		if(j == i)
		{
			j++;
		}

		bool positiveResult = n.Dot(points[j] - points[i]) >= 0;
		j++;
		for(; j < 4; j++)
		{
			if(j == i)
			{
				continue;
			}
			float dotResult = n.Dot(points[j] - points[i]);
			if((positiveResult && dotResult < 0) || (!positiveResult && dotResult > 0))
			{
				break;
			}
		}
		if(j == 4)
		{
			if(!positiveResult)
			{
				n = -n;
			}

			uint8_t normalIdx = 0;
			for(; normalIdx < numNormals; normalIdx++)
			{
				if(pNormals[normalIdx].Dot(n) > 0.99f)
				{
					break;
				}
			}

			// Skip if already stored this normal
			if(normalIdx != numNormals)
			{
				continue;
			}

			ASSERT(numNormals < kMaxFurstumNormal);
			pNormals[numNormals] = n;
			pointDotNormal[numNormals] = n.Dot(points[i]);
			numNormals++;
			if(m_pDebugDraw != nullptr)
			{
				const Vector3 colorWhite(1.0);
				m_pDebugDraw->AddLine(points[i], points[i + 4], colorWhite);
				m_pDebugDraw->AddLine(Vector3(points[i + 4] + (points[i] - points[i + 4]) / 2), Vector3(points[i + 4] + (points[i] - points[i + 4]) / 2 + n * 20.0f), colorWhite);
			}
		}
	}

	for(uint8_t i = 0; i < numNormals; i++)
	{
		if(pNormals[i].GetX() > 0.0f)
		{
			if(pNormals[i].GetZ() > 0.0f)
			{
				aabbNPVertex[i][0] = 0;
				aabbNPVertex[i][1] = 3;
			}
			else
			{
				aabbNPVertex[i][0] = 1;
				aabbNPVertex[i][1] = 2;
			}
		}
		else
		{
			if(pNormals[i].GetZ() > 0.0f)
			{
				aabbNPVertex[i][0] = 2;
				aabbNPVertex[i][1] = 1;
			}
			else
			{
				aabbNPVertex[i][0] = 3;
				aabbNPVertex[i][1] = 0;
			}
		}
	}
}

void QuadTreeSceneManager::TestVisibleLeafs(uint8_t numNormals, const Vector3* pNormals, const float* pPointDotNormal, const uint8_t(&aabbNPVertex)[kMaxFurstumNormal][2])
{
	for(QuadTreeLeaf* pLeaf : m_visibleLeafList)
	{
		EState state = IsAabbVisible(pLeaf->m_aabbPoints, numNormals, pNormals, pPointDotNormal, aabbNPVertex);
		if(state == EState::Outside)
		{
			SetLeafsVisible_recursive(*pLeaf, false);
		}
	}
	m_visibleLeafList.Clear();

	for(size_t i = 0; i < m_visibleDynamicComponents.Size(); ++i)
	{
		RenderComponent* pRenderComponent = m_visibleDynamicComponents[i];

		Vector3 points[4];
		pRenderComponent->GetAABB().GetPoints2D(points);

		EState state = IsAabbVisible(points, numNormals, pNormals, pPointDotNormal, aabbNPVertex);
		if(state == EState::Outside)
		{
			pRenderComponent->SetVisible(false);
			m_invisibleDynamicComponents.Add(pRenderComponent);
			m_visibleDynamicComponents.RemoveFastAt(i);
			i--;
		}
	}
}

void QuadTreeSceneManager::TestInvisibleLeafs(uint8_t numNormals, const Vector3* pNormals, const float* pPointDotNormal, const uint8_t(&aabbNPVertex)[kMaxFurstumNormal][2])
{
	ASSERT(m_visibleLeafList.IsEmpty());
	ASSERT(m_tmpLeafList.IsEmpty());
	QuadTreeLeaf* pLeaf = m_pRootLeaf;
	for(;;)
	{
		EState state = IsAabbVisible(pLeaf->m_aabbPoints, numNormals, pNormals, pPointDotNormal, aabbNPVertex);
		if(state == EState::Inside)
		{
			m_visibleLeafList.Add(pLeaf);
			SetLeafsVisible_recursive(*pLeaf, true);
		}
		else if(state == EState::Intersect)
		{
			m_visibleLeafList.Add(pLeaf);

			// Set objects visible
			for(RenderComponent* pRenderComponent : pLeaf->m_renderComponents)
			{
				pRenderComponent->SetVisible(true);
			}

			// Add child leafs to list
			for(uint8_t i = 0; i < 4; i++)
			{
				if(pLeaf->m_leafs[i] != nullptr)
				{
					m_tmpLeafList.Add(pLeaf->m_leafs[i]);
				}
			}
		}

		if(m_pDebugDraw != nullptr)
		{
			Vector3 color;
			switch(state)
			{
				case EState::Inside:
					color = Vector3(0.0f, 1.0f, 0.0f);
					break;

				case EState::Outside:
					color = Vector3(0.4f, 0.4f, 0.4f);
					// Draw a big X as well
					m_pDebugDraw->AddLine(pLeaf->m_aabbPoints[0], pLeaf->m_aabbPoints[3], color);
					m_pDebugDraw->AddLine(pLeaf->m_aabbPoints[1], pLeaf->m_aabbPoints[2], color);
					break;

				case EState::Intersect:
					color = Vector3(1.0f, 0.0f, 0.0f);
					break;
			}

			m_pDebugDraw->AddLine(pLeaf->m_aabbPoints[0], pLeaf->m_aabbPoints[1], color);
			m_pDebugDraw->AddLine(pLeaf->m_aabbPoints[2], pLeaf->m_aabbPoints[3], color);
			m_pDebugDraw->AddLine(pLeaf->m_aabbPoints[0], pLeaf->m_aabbPoints[2], color);
			m_pDebugDraw->AddLine(pLeaf->m_aabbPoints[1], pLeaf->m_aabbPoints[3], color);

			if(state != EState::Outside)
			{
				const Vector3 objectColor(0.0f, 0.5f, 1.0f);
				for(const RenderComponent* pRenderComponent : pLeaf->m_renderComponents)
				{
					m_pDebugDraw->AddAABB(pRenderComponent->GetAABB(), objectColor);
				}
			}
		}

		if(m_tmpLeafList.IsEmpty())
		{
			break;
		}

		pLeaf = m_tmpLeafList.GetLast();
		m_tmpLeafList.RemoveLast();
	}

	for(int32_t i = 0; i < static_cast<int>(m_invisibleDynamicComponents.Size()); ++i)
	{
		RenderComponent* pRenderComponent = m_invisibleDynamicComponents[i];
		Vector3 points[4];
		pRenderComponent->GetAABB().GetPoints2D(points);

		EState state = IsAabbVisible(points, numNormals, pNormals, pPointDotNormal, aabbNPVertex);
		if(state == EState::Inside || state == EState::Intersect)
		{
			pRenderComponent->SetVisible(true);
			m_visibleDynamicComponents.Add(pRenderComponent);
			m_invisibleDynamicComponents.RemoveFastAt(i);
			i--;
		}
	}
}

void QuadTreeSceneManager::SetLeafsVisible_recursive(QuadTreeLeaf& leaf, bool visible)
{
	for(RenderComponent* pRenderComponent : leaf.m_renderComponents)
	{
		pRenderComponent->SetVisible(visible);
	}

	for(uint8_t i = 0; i < 4; i++)
	{
		if(leaf.m_leafs[i] != nullptr)
		{
			SetLeafsVisible_recursive(*leaf.m_leafs[i], visible);
		}
	}
}

QuadTreeSceneManager::EState QuadTreeSceneManager::IsAabbVisible(const Vector3* pPoints, uint8_t numNormals, const Vector3* pNormals, const float* pPointDotNormal, const uint8_t(&aabbNPVertex)[kMaxFurstumNormal][2])
{
	EState rv = EState::Inside;
	for(uint8_t i = 0; i < numNormals; i++)
	{
		float dot = pNormals[i].Dot(pPoints[aabbNPVertex[i][0]]);
		if((dot - pPointDotNormal[i]) < 0.0f)
		{
			return EState::Outside;
		}

		float dot2 = pNormals[i].Dot(pPoints[aabbNPVertex[i][1]]);
		if((dot2 - pPointDotNormal[i]) < 0.0f)
		{
			rv = EState::Intersect;
		}
	}
	return rv;
}
