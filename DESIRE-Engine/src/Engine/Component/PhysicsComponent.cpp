#include "Engine/stdafx.h"
#include "Engine/Component/PhysicsComponent.h"
#include "Engine/Physics/Physics.h"				// for EPhysicsCollisionLayer
#include "Engine/Scene/Object.h"

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
