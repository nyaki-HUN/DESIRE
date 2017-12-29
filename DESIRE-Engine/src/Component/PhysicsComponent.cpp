#include "stdafx.h"
#include "Component/PhysicsComponent.h"
#include "Physics/IPhysics.h"
#include "Scene/Object.h"

PhysicsComponent::PhysicsComponent(Object& object)
	: Component(object)
	, collisionLayer(EPhysicsCollisionLayer::DEFAULT)
{
	AddToObject();
}

PhysicsComponent::~PhysicsComponent()
{

}

void PhysicsComponent::SetCollisionLayer(EPhysicsCollisionLayer i_collisionLayer)
{
	collisionLayer = i_collisionLayer;
}
