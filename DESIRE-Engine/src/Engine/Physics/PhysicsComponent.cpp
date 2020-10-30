#include "Engine/stdafx.h"
#include "Engine/Physics/PhysicsComponent.h"

#include "Engine/Physics/ColliderShape.h"
#include "Engine/Physics/Physics.h"

PhysicsComponent::PhysicsComponent(Object& object)
	: Component(object)
	, m_collisionLayer(EPhysicsCollisionLayer::Default)
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
	otherComponent.m_physicsMaterial = GetPhysicsMaterial();
	otherComponent.SetBodyType(GetBodyType());
	otherComponent.SetTrigger(IsTrigger());
	otherComponent.SetMass(GetMass());
	otherComponent.SetLinearDamping(GetLinearDamping());
	otherComponent.SetAngularDamping(GetAngularDamping());
	otherComponent.SetEnabled(IsEnabled());
}

void PhysicsComponent::SetCollisionLayer(EPhysicsCollisionLayer collisionLayer)
{
	m_collisionLayer = collisionLayer;
}

EPhysicsCollisionLayer PhysicsComponent::GetCollisionLayer() const
{
	return m_collisionLayer;
}

const PhysicsMaterial& PhysicsComponent::GetPhysicsMaterial() const
{
	return m_physicsMaterial;
}
