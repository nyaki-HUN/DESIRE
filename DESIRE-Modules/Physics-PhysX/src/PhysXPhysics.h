#pragma once

#include "Engine/Physics/Physics.h"

#include "../Externals/PhysX_3.4/Include/PxPhysicsAPI.h"

class PhysXPhysics : public Physics
{
public:
	PhysXPhysics();
	~PhysXPhysics() override;

	void Update(float deltaTime) override;

	PhysicsComponent& CreatePhysicsComponentOnObject(Object& object) override;

	bool RaycastClosest(const Vector3& p1, const Vector3& p2, PhysicsComponent **o_componentPtr, Vector3 *o_collisionPointPtr = nullptr, Vector3 *o_collisionNormalPtr = nullptr, int layerMask = Physics::MASK_ALL) override;
	bool RaycastAny(const Vector3& p1, const Vector3& p2, int layerMask = Physics::MASK_ALL) override;
	int RaycastAll(const Vector3& p1, const Vector3& p2, int maxCount, PhysicsComponent **o_components, Vector3 *o_collisionPoints = nullptr, Vector3 *o_collisionNormals = nullptr, int layerMask = Physics::MASK_ALL) override;

private:
	physx::PxFoundation *foundation = nullptr;
	physx::PxPhysics *physics = nullptr;
	physx::PxCooking *cooking = nullptr;
	physx::PxScene *scene = nullptr;

	physx::PxDefaultAllocator allocator;
	physx::PxDefaultErrorCallback errorCallback;
};
