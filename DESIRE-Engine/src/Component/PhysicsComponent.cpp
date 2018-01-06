#include "stdafx.h"
#include "Component/PhysicsComponent.h"
#include "Physics/Physics.h"			// for EPhysicsCollisionLayer
#include "Scene/Object.h"

PhysicsComponent::PhysicsComponent(Object& object)
	: Component(object)
	, collisionLayer(EPhysicsCollisionLayer::DEFAULT)
{

}

PhysicsComponent::~PhysicsComponent()
{

}

void PhysicsComponent::SetCollisionLayer(EPhysicsCollisionLayer i_collisionLayer)
{
	collisionLayer = i_collisionLayer;
}
