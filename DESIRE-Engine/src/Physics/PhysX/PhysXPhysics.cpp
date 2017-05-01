#include "stdafx.h"
#include "Physics/PhysX/PhysXPhysics.h"

#if defined(DESIRE_DEBUG)
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
	: foundation(nullptr)
	, physics(nullptr)
	, cooking(nullptr)
{
	foundation = PxCreateFoundation(PX_FOUNDATION_VERSION, allocator, errorCallback);
	if(foundation == nullptr)
	{
		LOG_ERROR("PxCreateFoundation failed!");
		return;
	}

	const bool recordMemoryAllocations = false;
	physx::PxTolerancesScale scale;
	physics = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation, scale, recordMemoryAllocations);
	if(physics == nullptr)
	{
		LOG_ERROR("PxCreatePhysics failed!");
		return;
	}

	physx::PxCookingParams params(scale);
	params.meshWeldTolerance = 0.001f;
	params.meshPreprocessParams = physx::PxMeshPreprocessingFlags(physx::PxMeshPreprocessingFlag::eWELD_VERTICES);
	params.buildGPUData = true;
	cooking = PxCreateCooking(PX_PHYSICS_VERSION, *foundation, params);
	if(cooking == nullptr)
	{
		LOG_ERROR("PxCreateCooking failed!");
		return;
	}

	physx::PxSceneDesc sceneDesc(physics->getTolerancesScale());
	sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
	sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
	sceneDesc.flags |= physx::PxSceneFlag::eENABLE_PCM;
	sceneDesc.flags |= physx::PxSceneFlag::eENABLE_STABILIZATION;
	sceneDesc.flags |= physx::PxSceneFlag::eSUPPRESS_EAGER_SCENE_QUERY_REFIT;
	sceneDesc.flags |= physx::PxSceneFlag::eENABLE_GPU_DYNAMICS;
	sceneDesc.broadPhaseType = physx::PxBroadPhaseType::eGPU;
	sceneDesc.gpuMaxNumPartitions = 8;

	scene = physics->createScene(sceneDesc);
}

PhysXPhysics::~PhysXPhysics()
{
	scene->release();
	cooking->release();
	physics->release();
	foundation->release();
}

void PhysXPhysics::Update()
{

}

PhysicsComponent* PhysXPhysics::CreatePhysicsComponent()
{
	return nullptr;
}

void PhysXPhysics::SetCollisionEnabled(EPhysicsCollisionGroup a, EPhysicsCollisionGroup b, bool enabled)
{

}

bool PhysXPhysics::RayTest(const Vector3& startPoint, const Vector3& direction, Vector3 *o_hitpoint, PhysicsComponent **o_component, int collisionGroupMask)
{
	DESIRE_UNUSED(startPoint);
	DESIRE_UNUSED(direction);
	DESIRE_UNUSED(o_hitpoint);
	DESIRE_UNUSED(o_component);
	DESIRE_UNUSED(collisionGroupMask);
	return false;
}
