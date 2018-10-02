#include "Engine/stdafx.h"
#include "Engine/Component/PhysicsComponent.h"
#include "Engine/Core/Modules.h"
#include "Engine/Physics/Physics.h"
#include "Engine/Physics/ColliderShape.h"

PhysicsComponent::PhysicsComponent(Object& object)
	: Component(object)
	, collisionLayer(EPhysicsCollisionLayer::Default)
{
	Modules::Physics->OnPhysicsComponentCreated(this);
}

PhysicsComponent::~PhysicsComponent()
{
	Modules::Physics->OnPhysicsComponentDestroyed(this);
}

void PhysicsComponent::SetCollisionLayer(EPhysicsCollisionLayer i_collisionLayer)
{
	collisionLayer = i_collisionLayer;
}

EPhysicsCollisionLayer PhysicsComponent::GetCollisionLayer() const
{
	return collisionLayer;
}

const PhysicsMaterial& PhysicsComponent::GetPhysicsMaterial() const
{
	return physicsMaterial;
}
