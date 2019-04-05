#include "PhysXPhysics.h"
#include "PhysXPhysicsComponent.h"
#include "PxMathExt.h"

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
	fixedUpdateTimeAccumulator += deltaTime;
	while(fixedUpdateTimeAccumulator >= fixedStepTime)
	{
		fixedUpdateTimeAccumulator -= fixedStepTime;

		scene->simulate(fixedStepTime);
		scene->fetchResults(true);

		// Instead of calling the UpdateComponents() function we update only the dynamic bodies that moved
		physx::PxU32 numActiveActors = 0;
		physx::PxActor **activeActors = scene->getActiveActors(numActiveActors);
		for(physx::PxU32 i = 0; i < numActiveActors; ++i)
		{
			PhysXPhysicsComponent *component = static_cast<PhysXPhysicsComponent*>(activeActors[i]->userData);
			component->SetTransformFromGameObject();
		}
	}

	// We still need to update all kinematic bodies
	for(PhysicsComponent *component : components)
	{
		if(component->GetBodyType() == PhysicsComponent::EBodyType::Kinematic)
		{
			component->SetTransformFromGameObject();
		}
	}
}

PhysicsComponent& PhysXPhysics::CreatePhysicsComponentOnObject(Object& object)
{
	PhysXPhysicsComponent& component = object.AddComponent<PhysXPhysicsComponent>();
	return component;
}

void PhysXPhysics::SetGravity(const Vector3& gravity)
{
	return scene->setGravity(GetPxVec3(gravity));
}

Vector3 PhysXPhysics::GetGravity() const
{
	return GetVector3(scene->getGravity());
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

Array<Collision> PhysXPhysics::RaycastAll(const Vector3& p1, const Vector3& p2, int layerMask)
{
	Array<Collision> collisions;
	return collisions;
}
