#pragma once

#include "Engine/Component/Component.h"
#include "Engine/Core/math/Vector3.h"
#include "Engine/Physics/PhysicsMaterial.h"

#include <memory>
#include <vector>

enum class EPhysicsCollisionLayer;
class ColliderShape;

class PhysicsComponent : public Component
{
	DECLARE_COMPONENT_FURCC_TYPE_ID("PHYS");

protected:
	PhysicsComponent(Object& object);

public:
	enum EBodyType
	{
		STATIC,
		DYNAMIC,
		KINEMATIC
	};

	enum EForceMode
	{
		FORCE,
		IMPULSE
	};

	~PhysicsComponent() override;

	// Collision
	virtual void SetCollisionLayer(EPhysicsCollisionLayer collisionLayer);
	EPhysicsCollisionLayer GetCollisionLayer() const;

	const PhysicsMaterial& GetPhysicsMaterial() const;

	virtual std::vector<PhysicsComponent*> GetActiveCollidingComponents() const = 0;

	virtual EBodyType GetBodyType() const = 0;
	virtual void SetBodyType(EBodyType value) = 0;

	virtual void SetTrigger(bool value) = 0;
	virtual bool IsTrigger() const = 0;

	// Mass
	virtual void SetMass(float mass) = 0;
	virtual float GetMass() const = 0;
	virtual Vector3 GetCenterOfMass() const = 0;

	// Damping
	virtual void SetLinearDamping(float value) = 0;
	virtual float GetLinearDamping() const = 0;
	virtual void SetAngularDamping(float value) = 0;
	virtual float GetAngularDamping() const = 0;

	// Velocity
	virtual void SetLinearVelocity(const Vector3& linearVelocity) = 0;
	virtual Vector3 GetLinearVelocity() const = 0;
	virtual void SetAngularVelocity(const Vector3& angularVelocity) = 0;
	virtual Vector3 GetAngularVelocity() const = 0;

	// Forces
	virtual void AddForce(const Vector3& force, EForceMode mode) = 0;
	virtual void AddForceAtPosition(const Vector3& force, const Vector3& position, EForceMode mode) = 0;
	virtual void AddTorque(const Vector3& torque, EForceMode mode) = 0;

	virtual bool IsSleeping() const = 0;

protected:
	EPhysicsCollisionLayer collisionLayer;
	PhysicsMaterial physicsMaterial;
	std::unique_ptr<ColliderShape> shape;
};
