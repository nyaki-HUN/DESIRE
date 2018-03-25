#include "PhysXPhysicsComponent.h"

#include "Engine/Core/assert.h"
#include "Engine/Core/Modules.h"
#include "Engine/Physics/Physics.h"

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
	ASSERT(false && "TODO");
}

float PhysXPhysicsComponent::GetMass() const
{
	ASSERT(false && "TODO");
	return 0.0f;
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
