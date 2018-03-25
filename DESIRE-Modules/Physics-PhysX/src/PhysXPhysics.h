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

	void SetGravity(const Vector3& gravity) override;
	Vector3 GetGravity() const override;

	Collision RaycastClosest(const Vector3& p1, const Vector3& p2, int layerMask = Physics::MASK_ALL) override;
	bool RaycastAny(const Vector3& p1, const Vector3& p2, int layerMask = Physics::MASK_ALL) override;
	std::vector<Collision> RaycastAll(const Vector3& p1, const Vector3& p2, int layerMask = Physics::MASK_ALL) override;

private:
	physx::PxFoundation *foundation = nullptr;
	physx::PxPhysics *physics = nullptr;
	physx::PxCooking *cooking = nullptr;
	physx::PxScene *scene = nullptr;

	physx::PxDefaultAllocator allocator;
	physx::PxDefaultErrorCallback errorCallback;
};
