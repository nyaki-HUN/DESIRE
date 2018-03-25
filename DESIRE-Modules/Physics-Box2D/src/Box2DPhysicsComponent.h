#pragma once

#include "Engine/Component/PhysicsComponent.h"
#include "Engine/Core/math/Vector2.h"

#include "Box2D/Dynamics/b2Fixture.h"

#include <vector>

class Shape;

class Box2DPhysicsComponent : public PhysicsComponent
{
public:
	Box2DPhysicsComponent(Object& object);
	~Box2DPhysicsComponent();

	void SetCollisionLayer(EPhysicsCollisionLayer collisionLayer) override;

	std::vector<PhysicsComponent*> GetActiveCollidingComponents() const override;

	void SetMass(float mass) override;
	float GetMass() const override;

	Vector3 GetCenterOfMass() const override;

	void SetDensity(float density);
	float GetDensity() const;

	void SetTrigger(bool value) override;
	bool IsTrigger() const override;

	float GetLinearDrag() const;
	void SetLinearDrag(float value);

	float GetAngularDrag() const;
	void SetAngularDrag(float value);

	Vector2 GetLinearVelocity();
	void SetLinearVelocity(const Vector2& velocity);

	float GetAngularVelocity() const;
	void SetAngularVelocity(float value);

	bool IsAwake() const;
	bool IsSleeping() const;

	void SetEnabled(bool value) override;

	b2Body* GetBody() const;

private:
	void CreateFixtures();
	void ReleaseFixtures();
	void UpdateFilterData();

	b2Body *body = nullptr;
	std::vector<b2Fixture*> fixtures;
	b2Filter filterData;
	float density = 1.0f;
	bool isTrigger = false;
};
