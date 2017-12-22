#pragma once

#include "Physics/IPhysics.h"

#include "../Externals/PhysX/include/PxPhysicsAPI.h"

class PhysXPhysics : public IPhysics
{
public:
	PhysXPhysics();
	~PhysXPhysics() override;

	void Update() override;

	PhysicsComponent* CreatePhysicsComponent() override;

	void SetCollisionEnabled(EPhysicsCollisionGroup a, EPhysicsCollisionGroup b, bool enabled) override;

	bool RayTest(const Vector3& startPoint, const Vector3& direction, Vector3 *o_hitpoint = nullptr, PhysicsComponent **o_component = nullptr, int collisionGroupMask = 0xffffffff) override;

private:
	physx::PxFoundation *foundation;
	physx::PxPhysics *physics;
	physx::PxCooking *cooking;
	physx::PxScene *scene;

	physx::PxDefaultAllocator allocator;
	physx::PxDefaultErrorCallback errorCallback;
};
