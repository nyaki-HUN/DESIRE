#include "stdafx.h"
#include "PhysXPhysicsComponent.h"

PhysXPhysicsComponent::PhysXPhysicsComponent(Object& object)
	: PhysicsComponent(object)
{

}

PhysXPhysicsComponent::~PhysXPhysicsComponent()
{

}

void PhysXPhysicsComponent::SetCollisionLayer(EPhysicsCollisionLayer i_collisionLayer)
{
	if(collisionLayer == i_collisionLayer)
	{
		return;
	}

	collisionLayer = i_collisionLayer;
}

void PhysXPhysicsComponent::SetMass(float mass)
{
	ASSERT(false && "TODO");
}

float PhysXPhysicsComponent::GetMass() const
{
	ASSERT(false && "TODO");
	return 0.0f;
}

bool PhysXPhysicsComponent::IsTrigger() const
{
	ASSERT(false && "TODO");
	return false;
}
