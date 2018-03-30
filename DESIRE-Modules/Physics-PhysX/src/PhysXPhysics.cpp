#include "PhysXPhysics.h"
#include "PhysXPhysicsComponent.h"

#include "Engine/Core/Log.h"
#include "Engine/Scene/Object.h"

#include "PxPhysicsAPI.h"

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
	allocator = std::make_unique<physx::PxDefaultAllocator>();
	errorCallback = std::make_unique<physx::PxDefaultErrorCallback>();

	foundation = PxCreateFoundation(PX_FOUNDATION_VERSION, *allocator.get(), *errorCallback.get());
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

void PhysXPhysics::Update(float deltaTime)
{
	UpdateComponents();
}

PhysicsComponent& PhysXPhysics::CreatePhysicsComponentOnObject(Object& object)
{
	PhysXPhysicsComponent& component = object.AddComponent<PhysXPhysicsComponent>();
	return component;
}

void PhysXPhysics::SetGravity(const Vector3& gravity)
{

}

Vector3 PhysXPhysics::GetGravity() const
{
	return Vector3::Zero();
}

Collision PhysXPhysics::RaycastClosest(const Vector3& p1, const Vector3& p2, int layerMask)
{
	Collision collision;
	return collision;
}

bool PhysXPhysics::RaycastAny(const Vector3& p1, const Vector3& p2, int layerMask)
{
	return false;
}

std::vector<Collision> PhysXPhysics::RaycastAll(const Vector3& p1, const Vector3& p2, int layerMask)
{
	std::vector<Collision> collisions;
	return collisions;
}
