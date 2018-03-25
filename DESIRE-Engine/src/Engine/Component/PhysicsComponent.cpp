#include "Engine/stdafx.h"
#include "Engine/Component/PhysicsComponent.h"
#include "Engine/Physics/Physics.h"				// for EPhysicsCollisionLayer
#include "Engine/Physics/ColliderShape.h"

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

const PhysicsMaterial& PhysicsComponent::GetPhysicsMaterial() const
{
	return physicsMaterial;
}
