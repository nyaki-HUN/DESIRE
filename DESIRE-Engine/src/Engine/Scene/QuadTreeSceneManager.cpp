#include "Engine/stdafx.h"
#include "Engine/Scene/QuadTreeSceneManager.h"
#include "Engine/Scene/QuadTreeLeaf.h"

#include "Engine/Core/Math/AABB.h"

#include "Engine/Render/Camera.h"
#include "Engine/Render/DebugDraw.h"
#include "Engine/Render/RenderComponent.h"

QuadTreeSceneManager::QuadTreeSceneManager()
	: rootLeaf(new QuadTreeLeaf(0))
	, numAllocatedVisibleDynamicComponents(512)
	, numAllocatedInvisibleDynamicComponents(512)
{
	visibleLeafList.Reserve(256);
	tmpLeafList.Reserve(256);

	visibleDynamicComponents = static_cast<RenderComponent**>(malloc(numAllocatedVisibleDynamicComponents * sizeof(RenderComponent*)));
	invisibleDynamicComponents = static_cast<RenderComponent**>(malloc(numAllocatedInvisibleDynamicComponents * sizeof(RenderComponent*)));
}

QuadTreeSceneManager::~QuadTreeSceneManager()
{
	delete rootLeaf;

	free(visibleDynamicComponents);
	free(invisibleDynamicComponents);
}

void QuadTreeSceneManager::Add(RenderComponent* component, bool dynamic)
{
	if(dynamic)
	{
		numVisibleDynamicComponents++;
		if(numVisibleDynamicComponents > numAllocatedVisibleDynamicComponents)
		{
			numAllocatedVisibleDynamicComponents *= 2;
			visibleDynamicComponents = static_cast<RenderComponent**>(realloc(visibleDynamicComponents, sizeof(RenderComponent*) * numAllocatedVisibleDynamicComponents));
		}
		visibleDynamicComponents[numVisibleDynamicComponents - 1] = component;
	}
	else
	{
		rootLeaf->Add(component);
		needToPlaceObjectsInsideQuadTree = true;
	}

	component->SetVisible(true);
}

void QuadTreeSceneManager::Remove(RenderComponent* component)
{
	for(uint32_t i = 0; i < numVisibleDynamicComponents; i++)
	{
		if(visibleDynamicComponents[i] == component)
		{
			visibleDynamicComponents[i] = visibleDynamicComponents[numVisibleDynamicComponents - 1];
			numVisibleDynamicComponents--;
			return;
		}
	}

	for(uint32_t i = 0; i < numInvisibleDynamicComponents; i++)
	{
		if(invisibleDynamicComponents[i] == component)
		{
			invisibleDynamicComponents[i] = invisibleDynamicComponents[numInvisibleDynamicComponents - 1];
			numInvisibleDynamicComponents--;
			return;
		}
	}

	rootLeaf->Remove(component);
}

void QuadTreeSceneManager::SetActiveCamera(Camera* camera)
{
	activeCamera = camera;

	// Set all dynamic objects visible
	for(uint32_t i = 0; i < numInvisibleDynamicComponents; i++)
	{
		invisibleDynamicComponents[i]->SetVisible(true);

		numVisibleDynamicComponents++;
		if(numVisibleDynamicComponents > numAllocatedVisibleDynamicComponents)
		{
			numAllocatedVisibleDynamicComponents *= 2;
			visibleDynamicComponents = static_cast<RenderComponent**>(realloc(visibleDynamicComponents, numAllocatedVisibleDynamicComponents * sizeof(RenderComponent*)));
		}

		visibleDynamicComponents[numVisibleDynamicComponents - 1] = invisibleDynamicComponents[i];
	}
	numInvisibleDynamicComponents = 0;

	// Set all leafs visible
	SetLeafsVisible_recursive(rootLeaf, true);

	visibleLeafList.Clear();
	doInvisibleLeafTest = true;
}

void QuadTreeSceneManager::Update()
{
	if(needToPlaceObjectsInsideQuadTree)
	{
		rootLeaf->Init();
		needToPlaceObjectsInsideQuadTree = false;
	}

	if(debugDraw != nullptr && doInvisibleLeafTest)
	{
		debugDraw->Reset();
	}

	uint8_t nNormal;
	Vector3 normals[MAX_FURSTUM_NORMAL];
	float pointDotNormal[MAX_FURSTUM_NORMAL];
	uint8_t aabbNPVertex[MAX_FURSTUM_NORMAL][2];
	CalcFrustumNormalsFromCamera(activeCamera, normals, nNormal, pointDotNormal, aabbNPVertex);

	if(doInvisibleLeafTest)
	{
		TestInvisibleLeafs(nNormal, normals, pointDotNormal, aabbNPVertex);
	}
	else
	{
		TestVisibleLeafs(nNormal, normals, pointDotNormal, aabbNPVertex);
	}

	doInvisibleLeafTest = !doInvisibleLeafTest;
}

void QuadTreeSceneManager::Reset()
{
	delete rootLeaf;
	rootLeaf = new QuadTreeLeaf(0);

	numVisibleDynamicComponents = 0;
	numInvisibleDynamicComponents = 0;

	visibleLeafList.Clear();

	doInvisibleLeafTest = true;
}

void QuadTreeSceneManager::CalcFrustumNormalsFromCamera(Camera* camera, Vector3* normals, uint8_t& nNormal, float(&pointDotNormal)[MAX_FURSTUM_NORMAL], uint8_t(&aabbNPVertex)[MAX_FURSTUM_NORMAL][2])
{
	nNormal = 0;

	Vector3 points[8];
	camera->CalculateFrustum(points);

	if(debugDraw != nullptr)
	{
		const Vector3 color(0.5f, 0.0f, 1.0f);

		// Far plane points
		debugDraw->AddLine(points[0], points[1], color);
		debugDraw->AddLine(points[1], points[3], color);
		debugDraw->AddLine(points[2], points[0], color);
		debugDraw->AddLine(points[3], points[2], color);
		// Near plane points
		debugDraw->AddLine(points[4], points[5], color);
		debugDraw->AddLine(points[5], points[7], color);
		debugDraw->AddLine(points[6], points[4], color);
		debugDraw->AddLine(points[7], points[6], color);

		debugDraw->AddLine(points[0], points[4], color);
		debugDraw->AddLine(points[1], points[5], color);
		debugDraw->AddLine(points[2], points[6], color);
		debugDraw->AddLine(points[3], points[7], color);
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
			for(; normalIdx < nNormal; normalIdx++)
			{
				if(normals[normalIdx].Dot(n) > 0.99f)
				{
					break;
				}
			}

			// Skip if already stored this normal
			if(normalIdx != nNormal)
			{
				continue;
			}

			ASSERT(nNormal < MAX_FURSTUM_NORMAL);
			normals[nNormal] = n;
			pointDotNormal[nNormal] = n.Dot(points[i]);
			nNormal++;
			if(debugDraw != nullptr)
			{
				const Vector3 colorWhite(1.0);
				debugDraw->AddLine(points[i], points[i + 4], colorWhite);
				debugDraw->AddLine(Vector3(points[i + 4] + (points[i] - points[i + 4]) / 2), Vector3(points[i + 4] + (points[i] - points[i + 4]) / 2 + n * 20.0f), colorWhite);
			}
		}
	}

	for(uint8_t i = 0; i < nNormal; i++)
	{
		if(normals[i].GetX() > 0.0f)
		{
			if(normals[i].GetZ() > 0.0f)
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
			if(normals[i].GetZ() > 0.0f)
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

void QuadTreeSceneManager::TestVisibleLeafs(uint8_t nNormal, const Vector3* normals, const float* pointDotNormal, const uint8_t(&aabbNPVertex)[MAX_FURSTUM_NORMAL][2])
{
	for(QuadTreeLeaf* ltmp : visibleLeafList)
	{
		EState state = IsAabbVisible(ltmp->aabbPoints, nNormal, normals, pointDotNormal, aabbNPVertex);
		if(state == EState::Outside)
		{
			SetLeafsVisible_recursive(ltmp, false);
		}
	}
	visibleLeafList.Clear();

	for(int j = 0; j < (int)numVisibleDynamicComponents; j++)
	{
		RenderComponent* component = visibleDynamicComponents[j];

		Vector3 points[4];
		component->GetAABB().GetPoints2D(points);

		EState state = IsAabbVisible(points, nNormal, normals, pointDotNormal, aabbNPVertex);
		if(state == EState::Outside)
		{
			component->SetVisible(false);

			numInvisibleDynamicComponents++;
			if(numInvisibleDynamicComponents > numAllocatedInvisibleDynamicComponents)
			{
				numAllocatedInvisibleDynamicComponents *= 2;
				invisibleDynamicComponents = static_cast<RenderComponent**>(realloc(invisibleDynamicComponents, numAllocatedInvisibleDynamicComponents * sizeof(RenderComponent*)));
			}
			invisibleDynamicComponents[numInvisibleDynamicComponents - 1] = component;
			numVisibleDynamicComponents--;
			visibleDynamicComponents[j] = visibleDynamicComponents[numVisibleDynamicComponents];
			j--;
		}
	}
}

void QuadTreeSceneManager::TestInvisibleLeafs(uint8_t nNormal, const Vector3* normals, const float* pointDotNormal, const uint8_t(&aabbNPVertex)[MAX_FURSTUM_NORMAL][2])
{
	ASSERT(visibleLeafList.IsEmpty());
	ASSERT(tmpLeafList.IsEmpty());
	QuadTreeLeaf* ltmp = rootLeaf;
	for(;;)
	{
		EState state = IsAabbVisible(ltmp->aabbPoints, nNormal, normals, pointDotNormal, aabbNPVertex);
		if(state == EState::Inside)
		{
			visibleLeafList.Add(ltmp);
			SetLeafsVisible_recursive(ltmp, true);
		}
		else if(state == EState::Intersect)
		{
			visibleLeafList.Add(ltmp);

			// Set objects visible
			for(RenderComponent* comp : ltmp->renderComponents)
			{
				comp->SetVisible(true);
			}

			// Add child leafs to list
			for(uint8_t i = 0; i < 4; i++)
			{
				if(ltmp->leafs[i] != nullptr)
				{
					tmpLeafList.Add(ltmp->leafs[i]);
				}
			}
		}

		if(debugDraw != nullptr)
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
					debugDraw->AddLine(ltmp->aabbPoints[0], ltmp->aabbPoints[3], color);
					debugDraw->AddLine(ltmp->aabbPoints[1], ltmp->aabbPoints[2], color);
					break;

				case EState::Intersect:
					color = Vector3(1.0f, 0.0f, 0.0f);
					break;
			}

			debugDraw->AddLine(ltmp->aabbPoints[0], ltmp->aabbPoints[1], color);
			debugDraw->AddLine(ltmp->aabbPoints[2], ltmp->aabbPoints[3], color);
			debugDraw->AddLine(ltmp->aabbPoints[0], ltmp->aabbPoints[2], color);
			debugDraw->AddLine(ltmp->aabbPoints[1], ltmp->aabbPoints[3], color);

			if(state != EState::Outside)
			{
				const Vector3 objectColor(0.0f, 0.5f, 1.0f);
				for(const RenderComponent* component : ltmp->renderComponents)
				{
					debugDraw->AddAABB(component->GetAABB(), objectColor);
				}
			}
		}

		if(tmpLeafList.IsEmpty())
		{
			break;
		}

		ltmp = tmpLeafList.GetLast();
		tmpLeafList.RemoveLast();
	}

	for(int j = 0; j < (int)numInvisibleDynamicComponents; j++)
	{
		RenderComponent* component = invisibleDynamicComponents[j];
		Vector3 points[4];
		component->GetAABB().GetPoints2D(points);

		EState state = IsAabbVisible(points, nNormal, normals, pointDotNormal, aabbNPVertex);
		if(state == EState::Inside || state == EState::Intersect)
		{
			component->SetVisible(true);

			numVisibleDynamicComponents++;
			if(numVisibleDynamicComponents > numAllocatedVisibleDynamicComponents)
			{
				numAllocatedVisibleDynamicComponents *= 2;
				visibleDynamicComponents = static_cast<RenderComponent**>(realloc(visibleDynamicComponents, numAllocatedVisibleDynamicComponents * sizeof(RenderComponent*)));
			}
			visibleDynamicComponents[numVisibleDynamicComponents - 1] = component;
			numInvisibleDynamicComponents--;
			invisibleDynamicComponents[j] = invisibleDynamicComponents[numInvisibleDynamicComponents];
			j--;
		}
	}
}

void QuadTreeSceneManager::SetLeafsVisible_recursive(QuadTreeLeaf* leaf, bool visible)
{
	for(RenderComponent* component : leaf->renderComponents)
	{
		component->SetVisible(visible);
	}

	for(uint8_t i = 0; i < 4; i++)
	{
		if(leaf->leafs[i] != nullptr)
		{
			SetLeafsVisible_recursive(leaf->leafs[i], visible);
		}
	}
}

QuadTreeSceneManager::EState QuadTreeSceneManager::IsAabbVisible(const Vector3* points, uint8_t nNormal, const Vector3* normals, const float* pointDotNormal, const uint8_t(&aabbNPVertex)[MAX_FURSTUM_NORMAL][2])
{
	EState rv = EState::Inside;
	for(uint8_t i = 0; i < nNormal; i++)
	{
		float dot = normals[i].Dot(points[aabbNPVertex[i][0]]);
		if((dot - pointDotNormal[i]) < 0.0f)
		{
			return EState::Outside;
		}

		float dot2 = normals[i].Dot(points[aabbNPVertex[i][1]]);
		if((dot2 - pointDotNormal[i]) < 0.0f)
		{
			rv = EState::Intersect;
		}
	}
	return rv;
}
