#pragma once

#include "Engine/Component/PhysicsComponent.h"

namespace physx
{
	class PxRigidDynamic;
}

class PhysXPhysicsComponent : public PhysicsComponent
{
public:
	PhysXPhysicsComponent(Object& object);
	~PhysXPhysicsComponent();

	void SetCollisionLayer(EPhysicsCollisionLayer collisionLayer) override;

	std::vector<PhysicsComponent*> GetActiveCollidingComponents() const override;

	void SetMass(float mass) override;
	float GetMass() const override;

	Vector3 GetCenterOfMass() const override;

	void SetTrigger(bool value) override;
	bool IsTrigger() const override;

	void SetLinearDamping(float value) override;
	float GetLinearDamping() const override;
	void SetAngularDamping(float value) override;
	float GetAngularDamping() const override;

private:
	physx::PxRigidDynamic *body = nullptr;
};
