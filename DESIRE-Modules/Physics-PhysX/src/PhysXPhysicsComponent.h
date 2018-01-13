#pragma once

#include "Component/PhysicsComponent.h"

class PhysXPhysicsComponent : public PhysicsComponent
{
public:
	PhysXPhysicsComponent(Object& object);
	~PhysXPhysicsComponent();

	void SetCollisionLayer(EPhysicsCollisionLayer collisionLayer) override;

	void SetMass(float mass) override;
	float GetMass() const override;

	bool IsTrigger() const override;

private:
};
