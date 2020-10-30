#pragma once

#include "Engine/Core/Component.h"
#include "Engine/Core/Container/Array.h"
#include "Engine/Core/Math/Vector3.h"

#include "Engine/Physics/PhysicsMaterial.h"

class ColliderShape;
enum class EPhysicsCollisionLayer;

class PhysicsComponent : public Component
{
	DECLARE_COMPONENT_FOURCC_TYPE_ID("PHYS");

protected:
	PhysicsComponent(Object& object);

public:
	enum class EBodyType
	{
		Static,
		Dynamic,
		Kinematic
	};

	enum class ECollisionDetectionMode
	{
		Discrete,
		Continuous
	};

	enum class EForceMode
	{
		Force,
		Impulse
	};

	~PhysicsComponent() override;

	virtual void CloneTo(Object& otherObject) const override;

	// Collision
	virtual void SetCollisionLayer(EPhysicsCollisionLayer collisionLayer);
	EPhysicsCollisionLayer GetCollisionLayer() const;

	virtual void SetCollisionDetectionMode(ECollisionDetectionMode mode) = 0;
	virtual ECollisionDetectionMode GetCollisionDetectionMode() const = 0;

	const PhysicsMaterial& GetPhysicsMaterial() const;

	virtual Array<PhysicsComponent*> GetActiveCollidingComponents() const = 0;

	virtual void SetBodyType(EBodyType bodyType) = 0;
	virtual EBodyType GetBodyType() const = 0;

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

	// Motion lock
	virtual void SetLinearMotionLock(bool axisX, bool axisY, bool axisZ) = 0;
	virtual void SetAngularMotionLock(bool axisX, bool axisY, bool axisZ) = 0;

	virtual bool IsSleeping() const = 0;

	virtual void UpdateGameObjectTransform() const = 0;
	virtual void SetTransformFromGameObject() = 0;

protected:
	EPhysicsCollisionLayer m_collisionLayer;
	PhysicsMaterial m_physicsMaterial;
	std::unique_ptr<ColliderShape> m_spShape;
};
