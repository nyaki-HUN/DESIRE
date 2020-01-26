#pragma once

#include "Engine/Physics/PhysicsComponent.h"

#include "Engine/Core/Container/Array.h"
#include "Engine/Core/Math/Vector2.h"

#include "box2d/b2_fixture.h"

class Shape;

class Box2DPhysicsComponent : public PhysicsComponent
{
public:
	Box2DPhysicsComponent(Object& object);
	~Box2DPhysicsComponent();

	void SetEnabled(bool value) override;
	void CloneTo(Object& otherObject) const override;

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

	void SetDensity(float density);
	float GetDensity() const;

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

	b2Body* GetBody() const;

private:
	void CreateFixtures();
	void ReleaseFixtures();

	b2Body* body = nullptr;
	Array<b2Fixture*> fixtures;
	b2Filter filterData;
	float density = 1.0f;
	bool isTrigger = false;
};
