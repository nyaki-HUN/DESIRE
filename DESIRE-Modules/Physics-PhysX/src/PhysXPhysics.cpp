#include "PhysXPhysics.h"
#include "PhysXPhysicsComponent.h"

#include "Engine/Core/Log.h"
#include "Engine/Scene/Object.h"

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
	if(scene != nullptr)
	{
		scene->release();
		scene = nullptr;
	}

	if(cooking != nullptr)
	{
		cooking->release();
		cooking = nullptr;
	}

	if(physics != nullptr)
	{
		physics->release();
		physics = nullptr;
	}

	if(foundation != nullptr)
	{
		foundation->release();
		foundation = nullptr;
	}
}

void PhysXPhysics::Update()
{

}

PhysicsComponent& PhysXPhysics::CreatePhysicsComponentOnObject(Object& object)
{
	PhysXPhysicsComponent& component = object.AddComponent<PhysXPhysicsComponent>();
	return component;
}

bool PhysXPhysics::RaycastClosest(const Vector3& p1, const Vector3& p2, PhysicsComponent **o_componentPtr, Vector3 *o_collisionPointPtr, Vector3 *o_collisionNormalPtr, int layerMask)
{
	return false;
}

bool PhysXPhysics::RaycastAny(const Vector3& p1, const Vector3& p2, int layerMask)
{
	return false;
}

int PhysXPhysics::RaycastAll(const Vector3& p1, const Vector3& p2, int maxCount, PhysicsComponent **o_components, Vector3 *o_collisionPoints, Vector3 *o_collisionNormals, int layerMask)
{
	return 0;
}
