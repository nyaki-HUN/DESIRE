#include "PhysXPhysicsComponent.h"

#include "Engine/Core/assert.h"
#include "Engine/Core/Modules.h"
#include "Engine/Physics/Physics.h"

#include "PxRigidDynamic.h"
#include "PxFiltering.h"

PhysXPhysicsComponent::PhysXPhysicsComponent(Object& object)
	: PhysicsComponent(object)
{

}

PhysXPhysicsComponent::~PhysXPhysicsComponent()
{

}

void PhysXPhysicsComponent::SetCollisionLayer(EPhysicsCollisionLayer i_collisionLayer)
{
	collisionLayer = i_collisionLayer;

	physx::PxFilterData filterData;
	filterData.word0 = 1 << (int)collisionLayer;
	filterData.word1 = Modules::Physics->GetMaskForCollisionLayer(collisionLayer);
}

std::vector<PhysicsComponent*> PhysXPhysicsComponent::GetActiveCollidingComponents() const
{
	std::vector<PhysicsComponent*> collisions;
	ASSERT(false && "TODO");
	return collisions;
}

void PhysXPhysicsComponent::SetMass(float mass)
{
	return body->setMass(mass);
}

float PhysXPhysicsComponent::GetMass() const
{
	return body->getMass();
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

void PhysXPhysicsComponent::SetLinearDamping(float value)
{
	body->setLinearDamping(value);
}

float PhysXPhysicsComponent::GetLinearDamping() const
{
	return body->getLinearDamping();
}

void PhysXPhysicsComponent::SetAngularDamping(float value)
{
	body->setAngularDamping(value);
}

float PhysXPhysicsComponent::GetAngularDamping() const
{
	return body->getAngularDamping();
}
