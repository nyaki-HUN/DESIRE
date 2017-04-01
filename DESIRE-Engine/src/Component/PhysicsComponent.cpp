#include "stdafx.h"
#include "Component/PhysicsComponent.h"
#include "Physics/IPhysics.h"

PhysicsComponent::PhysicsComponent()
	: collisionGroup(EPhysicsCollisionGroup::DEFAULT)
{

}

PhysicsComponent::~PhysicsComponent()
{

}

void PhysicsComponent::SetCollisionGroup(EPhysicsCollisionGroup i_collisionGroup)
{
	collisionGroup = i_collisionGroup;
}
