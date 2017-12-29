#include "stdafx.h"
#include "Component/PhysicsComponent.h"
#include "Physics/IPhysics.h"

PhysicsComponent::PhysicsComponent()
	: collisionLayer(EPhysicsCollisionLayer::DEFAULT)
{

}

PhysicsComponent::~PhysicsComponent()
{

}

void PhysicsComponent::SetCollisionLayer(EPhysicsCollisionLayer i_collisionLayer)
{
	collisionLayer = i_collisionLayer;
}
