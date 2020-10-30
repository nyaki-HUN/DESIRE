#pragma once

#include "Engine/Physics/Physics.h"

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
	physx::PxFoundation* m_pFoundation = nullptr;
	physx::PxPhysics* m_pPhysics = nullptr;
	physx::PxCooking* m_pCooking = nullptr;
	physx::PxScene* m_pScene = nullptr;

	std::unique_ptr<physx::PxAllocatorCallback> m_spAllocator;
	std::unique_ptr<physx::PxErrorCallback> m_spErrorCallback;

	float m_fixedUpdateTimeAccumulator = 0.0f;
};
