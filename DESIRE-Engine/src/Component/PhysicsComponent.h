#pragma once

#include "Component/IComponent.h"

enum class EPhysicsCollisionGroup;

class PhysicsComponent : public IComponent
{
	DECLARE_COMPONENT_FURCC_TYPE_ID("PHYS");

public:
	PhysicsComponent();
	~PhysicsComponent() override;

	virtual void SetCollisionGroup(EPhysicsCollisionGroup collisionGroup);
	virtual void SetMass(float mass) = 0;
	virtual float GetMass() const = 0;

protected:
	EPhysicsCollisionGroup collisionGroup;
};
