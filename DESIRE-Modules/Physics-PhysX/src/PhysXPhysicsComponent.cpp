#include "PhysXPhysicsComponent.h"

#include "Engine/Core/assert.h"

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

std::vector<PhysicsComponent*> PhysXPhysicsComponent::GetActiveCollidingComponents() const
{
	std::vector<PhysicsComponent*> collisions;
	ASSERT(false && "TODO");
	return collisions;
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

Vector3 PhysXPhysicsComponent::GetCenterOfMass() const
{
	ASSERT(false && "TODO");
	return Vector3(0.0f);
}

void PhysXPhysicsComponent::SetTrigger(bool value)
{
	ASSERT(false && "TODO");
}

bool PhysXPhysicsComponent::IsTrigger() const
{
	ASSERT(false && "TODO");
	return false;
}
