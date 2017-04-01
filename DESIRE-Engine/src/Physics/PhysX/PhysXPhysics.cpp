#include "stdafx.h"
#include "Physics/PhysX/PhysXPhysics.h"

PhysXPhysics::PhysXPhysics()
{

}

PhysXPhysics::~PhysXPhysics()
{

}

void PhysXPhysics::Update()
{

}

PhysicsComponent* PhysXPhysics::CreatePhysicsComponent()
{
	return nullptr;
}

bool PhysXPhysics::RayTest(const Vector3& startPoint, const Vector3& direction, Vector3 *o_hitpoint, PhysicsComponent **o_component, int collisionGroupMask)
{
	DESIRE_UNUSED(startPoint);
	DESIRE_UNUSED(direction);
	DESIRE_UNUSED(o_hitpoint);
	DESIRE_UNUSED(o_component);
	DESIRE_UNUSED(collisionGroupMask);
	return false;
}
