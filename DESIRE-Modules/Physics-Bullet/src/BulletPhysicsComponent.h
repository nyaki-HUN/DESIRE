#pragma once

#include "Engine/Component/PhysicsComponent.h"

class btRigidBody;
class btCollisionShape;
struct btDefaultMotionState;
class btTriangleIndexVertexArray;

class BulletPhysicsComponent : public PhysicsComponent
{
public:
	BulletPhysicsComponent(Object& object, bool dynamic);
	~BulletPhysicsComponent();

	// Collision
	void SetCollisionLayer(EPhysicsCollisionLayer collisionLayer) override;

	std::vector<PhysicsComponent*> GetActiveCollidingComponents() const override;

	EBodyType GetBodyType() const;
	void SetBodyType(EBodyType bodyType);

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

	bool IsSleeping() const override;

	void UpdateGameObjectTransform() const override;
	void SetTransformFromGameObject() override;

	void SetEnabled(bool value) override;

private:
	btRigidBody *body = nullptr;
	btCollisionShape *shape = nullptr;
	btDefaultMotionState *motionState = nullptr;
	btTriangleIndexVertexArray *triangleIndexVertexArrays = nullptr;

	bool dynamic = false;
};
