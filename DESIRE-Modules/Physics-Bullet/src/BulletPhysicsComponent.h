#pragma once

#include "Engine/Physics/PhysicsComponent.h"

class btRigidBody;
struct btDefaultMotionState;
class btTriangleIndexVertexArray;

class BulletPhysicsComponent : public PhysicsComponent
{
public:
	BulletPhysicsComponent(Object& object, bool dynamic);
	~BulletPhysicsComponent();

	void SetEnabled(bool value) override;

	// Collision
	void SetCollisionLayer(EPhysicsCollisionLayer collisionLayer) override;

	void SetCollisionDetectionMode(ECollisionDetectionMode mode) override;
	ECollisionDetectionMode GetCollisionDetectionMode() const override;

	Array<PhysicsComponent*> GetActiveCollidingComponents() const override;

	void SetBodyType(EBodyType bodyType);
	EBodyType GetBodyType() const;

	void SetTrigger(bool value) override;
	bool IsTrigger() const override;

	// Mass
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

	// Motion lock
	void SetLinearMotionLock(bool axisX, bool axisY, bool axisZ) override;
	void SetAngularMotionLock(bool axisX, bool axisY, bool axisZ) override;

	bool IsSleeping() const override;

	void UpdateGameObjectTransform() const override;
	void SetTransformFromGameObject() override;

private:
	btRigidBody* body = nullptr;
	btDefaultMotionState* motionState = nullptr;
	btTriangleIndexVertexArray* triangleIndexVertexArrays = nullptr;

	bool dynamic = false;
};
