#include "stdafx_PhysX.h"
#include "PhysXPhysics.h"

#include "PhysXCustomAllocator.h"
#include "PhysXPhysicsComponent.h"
#include "PxMathExt.h"

#include "Engine/Core/Object.h"

#if defined(_DEBUG)
	#pragma comment(lib, "PhysX3DEBUG_x64.lib")
	#pragma comment(lib, "PhysX3CookingDEBUG_x64.lib")
	#pragma comment(lib, "PhysX3ExtensionsDEBUG.lib")
	#pragma comment(lib, "PxFoundationDEBUG_x64.lib")
#else
	#pragma comment(lib, "PhysX3_x64.lib")
	#pragma comment(lib, "PhysX3Cooking_x64.lib")
	#pragma comment(lib, "PhysX3Extensions.lib")
	#pragma comment(lib, "PxFoundation_x64.lib")
#endif

PhysXPhysics::PhysXPhysics()
{
	m_spAllocator = std::make_unique<PhysXCustomAllocator>();
	m_spErrorCallback = std::make_unique<physx::PxDefaultErrorCallback>();

	m_pFoundation = PxCreateFoundation(PX_FOUNDATION_VERSION, *m_spAllocator, *m_spErrorCallback);
	if(m_pFoundation == nullptr)
	{
		LOG_ERROR("PxCreateFoundation failed!");
		return;
	}

	const bool recordMemoryAllocations = false;
	physx::PxTolerancesScale scale;
	m_pPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_pFoundation, scale, recordMemoryAllocations);
	if(m_pPhysics == nullptr)
	{
		LOG_ERROR("PxCreatePhysics failed!");
		return;
	}

	physx::PxCookingParams params(scale);
	params.meshWeldTolerance = 0.001f;
	params.meshPreprocessParams = physx::PxMeshPreprocessingFlags(physx::PxMeshPreprocessingFlag::eWELD_VERTICES);
	params.buildGPUData = true;
	m_pCooking = PxCreateCooking(PX_PHYSICS_VERSION, *m_pFoundation, params);
	if(m_pCooking == nullptr)
	{
		LOG_ERROR("PxCreateCooking failed!");
		return;
	}

	physx::PxSceneDesc sceneDesc(m_pPhysics->getTolerancesScale());
	sceneDesc.gravity = physx::PxVec3(0.0f, -9.8f, 0.0f);
	sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
	sceneDesc.flags |= physx::PxSceneFlag::eENABLE_ACTIVE_ACTORS;
	sceneDesc.flags |= physx::PxSceneFlag::eENABLE_CCD;
	sceneDesc.flags |= physx::PxSceneFlag::eENABLE_PCM;
	sceneDesc.flags |= physx::PxSceneFlag::eENABLE_STABILIZATION;
	sceneDesc.flags |= physx::PxSceneFlag::eEXCLUDE_KINEMATICS_FROM_ACTIVE_ACTORS;
	sceneDesc.flags |= physx::PxSceneFlag::eSUPPRESS_EAGER_SCENE_QUERY_REFIT;
	sceneDesc.flags |= physx::PxSceneFlag::eENABLE_GPU_DYNAMICS;
	sceneDesc.broadPhaseType = physx::PxBroadPhaseType::eGPU;
	sceneDesc.gpuMaxNumPartitions = 8;

	m_pScene = m_pPhysics->createScene(sceneDesc);
}

PhysXPhysics::~PhysXPhysics()
{
	if(m_pScene)
	{
		m_pScene->release();
		m_pScene = nullptr;
	}

	if(m_pCooking)
	{
		m_pCooking->release();
		m_pCooking = nullptr;
	}

	if(m_pPhysics)
	{
		m_pPhysics->release();
		m_pPhysics = nullptr;
	}

	if(m_pFoundation)
	{
		m_pFoundation->release();
		m_pFoundation = nullptr;
	}
}

void PhysXPhysics::Update(float deltaTime)
{
	m_fixedUpdateTimeAccumulator += deltaTime;
	while(m_fixedUpdateTimeAccumulator >= m_fixedStepTime)
	{
		m_fixedUpdateTimeAccumulator -= m_fixedStepTime;

		m_pScene->simulate(m_fixedStepTime);
		m_pScene->fetchResults(true);

		// Instead of calling the UpdateComponents() function we update only the dynamic bodies that moved
		physx::PxU32 numActiveActors = 0;
		physx::PxActor** ppActiveActors = m_pScene->getActiveActors(numActiveActors);
		for(physx::PxU32 i = 0; i < numActiveActors; ++i)
		{
			PhysXPhysicsComponent* pPhysicsComponent = static_cast<PhysXPhysicsComponent*>(ppActiveActors[i]->userData);
			pPhysicsComponent->SetTransformFromGameObject();
		}
	}

	// We still need to update all kinematic bodies
	for(PhysicsComponent* pPhysicsComponent : m_components)
	{
		if(pPhysicsComponent->GetBodyType() == PhysicsComponent::EBodyType::Kinematic)
		{
			pPhysicsComponent->SetTransformFromGameObject();
		}
	}
}

PhysicsComponent& PhysXPhysics::CreatePhysicsComponentOnObject(Object& object)
{
	PhysXPhysicsComponent& physicsComponent = object.AddComponent<PhysXPhysicsComponent>();
	return physicsComponent;
}

void PhysXPhysics::SetGravity(const Vector3& gravity)
{
	return m_pScene->setGravity(GetPxVec3(gravity));
}

Vector3 PhysXPhysics::GetGravity() const
{
	return GetVector3(m_pScene->getGravity());
}

Collision PhysXPhysics::RaycastClosest(const Vector3& p1, const Vector3& p2, int32_t layerMask)
{
	Collision collision;
	return collision;
}

bool PhysXPhysics::RaycastAny(const Vector3& p1, const Vector3& p2, int32_t layerMask)
{
	return false;
}

Array<Collision> PhysXPhysics::RaycastAll(const Vector3& p1, const Vector3& p2, int32_t layerMask)
{
	Array<Collision> collisions;
	return collisions;
}
