#pragma once

#include "Engine/Component/PhysicsComponent.h"

namespace physx
{
	class PxRigidBody;
	class PxRigidDynamic;
}

class PhysXPhysicsComponent : public PhysicsComponent
{
public:
	PhysXPhysicsComponent(Object& object);
	~PhysXPhysicsComponent();

	// Collision
	void SetCollisionLayer(EPhysicsCollisionLayer collisionLayer) override;

	std::vector<PhysicsComponent*> GetActiveCollidingComponents() const override;

	EBodyType GetBodyType() const;
	void SetBodyType(EBodyType bodyType);

	void SetTrigger(bool value) override;
	bool IsTrigger() const override;

	// Mass manipulation
	void SetMass(float mass) override;
	float GetMass() const override;
	Vector3 GetCenterOfMass() const override;

	// Damping
	void SetLinearDamping(float value) override;
	float GetLinearDamping() const override;
	void SetAngularDamping(float value) override;
	float GetAngularDamping() const override;

	// Velocity
	void SetLinearVelocity(const Vector3& linearVelocity) override;
	Vector3 GetLinearVelocity() const override;
	void SetAngularVelocity(const Vector3& angularVelocity) override;
	Vector3 GetAngularVelocity() const override;

	// Forces
	void AddForce(const Vector3& force, EForceMode mode) override;
	void AddForceAtPosition(const Vector3& force, const Vector3& position, EForceMode mode) override;
	void AddTorque(const Vector3& torque, EForceMode mode) override;

private:
	physx::PxRigidBody *body = nullptr;
	physx::PxRigidDynamic *dynamicBody = nullptr;
};
