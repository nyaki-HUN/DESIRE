#pragma once

#include "Component/IComponent.h"

enum class EPhysicsCollisionLayer;

class PhysicsComponent : public IComponent
{
	DECLARE_COMPONENT_FURCC_TYPE_ID("PHYS");

public:
	PhysicsComponent();
	~PhysicsComponent() override;

	virtual void SetCollisionLayer(EPhysicsCollisionLayer collisionLayer);

	virtual void SetMass(float mass) = 0;
	virtual float GetMass() const = 0;

protected:
	EPhysicsCollisionLayer collisionLayer;
};
