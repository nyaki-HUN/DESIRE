#pragma once

#include "Component/Component.h"

enum class EPhysicsCollisionLayer;

class PhysicsComponent : public Component
{
	DECLARE_COMPONENT_FURCC_TYPE_ID("PHYS");

protected:
	PhysicsComponent(Object& object);

public:
	~PhysicsComponent() override;

	virtual void SetCollisionLayer(EPhysicsCollisionLayer collisionLayer);

	virtual void SetMass(float mass) = 0;
	virtual float GetMass() const = 0;

	virtual bool IsTrigger() const = 0;

protected:
	EPhysicsCollisionLayer collisionLayer;
};
