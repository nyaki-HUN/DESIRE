#pragma once

#include "Engine/Component/PhysicsComponent.h"

#include "Box2D/Dynamics/b2Fixture.h"
#include <vector>

class Box2DPhysicsComponent : public PhysicsComponent
{
public:
	Box2DPhysicsComponent(Object& object);
	~Box2DPhysicsComponent();

	void SetCollisionLayer(EPhysicsCollisionLayer collisionLayer) override;

	void SetMass(float mass) override;
	float GetMass() const override;

	bool IsTrigger() const override;

private:
	void ReleaseFixtures();
	void UpdateFilterData();

	b2Body* body = nullptr;
	std::vector<b2Fixture*> fixtures;
	b2Filter filterData;

	bool isTrigger = false;
};
