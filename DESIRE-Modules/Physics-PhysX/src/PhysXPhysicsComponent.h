#pragma once

#include "Engine/Component/PhysicsComponent.h"

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

private:
};
