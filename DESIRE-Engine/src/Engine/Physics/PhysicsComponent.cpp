#include "Engine/stdafx.h"
#include "Engine/Physics/PhysicsComponent.h"
#include "Engine/Physics/Physics.h"
#include "Engine/Physics/ColliderShape.h"
#include "Engine/Core/Modules.h"

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

void PhysicsComponent::CloneTo(Object& otherObject) const
{
	PhysicsComponent& otherComponent = Modules::Physics->CreatePhysicsComponentOnObject(otherObject);
	otherComponent.SetCollisionLayer(GetCollisionLayer());
	otherComponent.SetCollisionDetectionMode(GetCollisionDetectionMode());
	otherComponent.physicsMaterial = GetPhysicsMaterial();
	otherComponent.SetBodyType(GetBodyType());
	otherComponent.SetTrigger(IsTrigger());
	otherComponent.SetMass(GetMass());
	otherComponent.SetLinearDamping(GetLinearDamping());
	otherComponent.SetAngularDamping(GetAngularDamping());
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
