#pragma once

#include "Engine/Physics/Physics.h"

#include <memory>

namespace physx
{
	class PxFoundation;
	class PxPhysics;
	class PxCooking;
	class PxScene;
	class PxAllocatorCallback;
	class PxErrorCallback;
}

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
	Array<Collision> RaycastAll(const Vector3& p1, const Vector3& p2, int layerMask = Physics::MASK_ALL) override;

private:
	physx::PxFoundation *foundation = nullptr;
	physx::PxPhysics *physics = nullptr;
	physx::PxCooking *cooking = nullptr;
	physx::PxScene *scene = nullptr;

	std::unique_ptr<physx::PxAllocatorCallback> allocator;
	std::unique_ptr<physx::PxErrorCallback> errorCallback;

	float fixedUpdateTimeAccumulator = 0.0f;
};
