#include "PhysXPhysicsComponent.h"
#include "PxMathExt.h"

#include "Engine/Core/assert.h"
#include "Engine/Core/Modules.h"
#include "Engine/Physics/Physics.h"

#include "PxRigidDynamic.h"
#include "PxFiltering.h"
#include "extensions/PxRigidBodyExt.h"

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

PhysicsComponent::EBodyType PhysXPhysicsComponent::GetBodyType() const
{
	const physx::PxRigidBodyFlags flags = body->getRigidBodyFlags();
	if(flags.isSet(physx::PxRigidBodyFlag::eKINEMATIC))
	{
		return PhysicsComponent::EBodyType::KINEMATIC;
	}

	if(dynamicBody == nullptr)
	{
		PhysicsComponent::EBodyType::STATIC;
	}

	return PhysicsComponent::EBodyType::DYNAMIC;
}

void PhysXPhysicsComponent::SetBodyType(EBodyType bodyType)
{
	switch(bodyType)
	{
		case EBodyType::STATIC:		ASSERT(false && "TODO");  break;
		case EBodyType::DYNAMIC:	body->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, false); break;
		case EBodyType::KINEMATIC:	body->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, true); break;
	}
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
	body->getCMassLocalPose();
	ASSERT(false && "TODO");
	return Vector3::Zero();
}

void PhysXPhysicsComponent::SetLinearDamping(float value)
{
	if(dynamicBody != nullptr)
	{
		dynamicBody->setLinearDamping(value);
	}
}

float PhysXPhysicsComponent::GetLinearDamping() const
{
	return (dynamicBody != nullptr) ? dynamicBody->getLinearDamping() : 0.0f;
}

void PhysXPhysicsComponent::SetAngularDamping(float value)
{
	if(dynamicBody != nullptr)
	{
		dynamicBody->setAngularDamping(value);
	}
}

float PhysXPhysicsComponent::GetAngularDamping() const
{
	return (dynamicBody != nullptr) ? dynamicBody->getAngularDamping() : 0.0f;
}

void PhysXPhysicsComponent::SetLinearVelocity(const Vector3& linearVelocity)
{
	body->setLinearVelocity(GetPxVec3(linearVelocity));
}

Vector3 PhysXPhysicsComponent::GetLinearVelocity() const
{
	return GetVector3(body->getLinearVelocity());
}

void PhysXPhysicsComponent::SetAngularVelocity(const Vector3& angularVelocity)
{
	body->setAngularVelocity(GetPxVec3(angularVelocity));
}

Vector3 PhysXPhysicsComponent::GetAngularVelocity() const
{
	return GetVector3(body->getAngularVelocity());
}

void PhysXPhysicsComponent::AddForce(const Vector3& force, EForceMode mode)
{
	switch(mode)
	{
		case EForceMode::FORCE:		body->addForce(GetPxVec3(force), physx::PxForceMode::eFORCE); break;
		case EForceMode::IMPULSE:	body->addForce(GetPxVec3(force), physx::PxForceMode::eIMPULSE); break;
	}
}

void PhysXPhysicsComponent::AddForceAtPosition(const Vector3& force, const Vector3& position, EForceMode mode)
{
	switch(mode)
	{
		case EForceMode::FORCE:		physx::PxRigidBodyExt::addForceAtPos(*body, GetPxVec3(force), GetPxVec3(position), physx::PxForceMode::eFORCE);
		case EForceMode::IMPULSE:	physx::PxRigidBodyExt::addForceAtPos(*body, GetPxVec3(force), GetPxVec3(position), physx::PxForceMode::eIMPULSE);
	}
}

void PhysXPhysicsComponent::AddTorque(const Vector3& torque, EForceMode mode)
{
	switch(mode)
	{
		case EForceMode::FORCE:		body->addTorque(GetPxVec3(torque), physx::PxForceMode::eFORCE); break;
		case EForceMode::IMPULSE:	body->addTorque(GetPxVec3(torque), physx::PxForceMode::eIMPULSE); break;
	}
}

bool PhysXPhysicsComponent::IsSleeping() const
{
	return (dynamicBody != nullptr) ? dynamicBody->isSleeping() : true;
}

void PhysXPhysicsComponent::SetEnabled(bool value)
{
	if(IsEnabled() == value)
	{
		return;
	}

	PhysicsComponent::SetEnabled(value);
	body->setActorFlag(physx::PxActorFlag::eDISABLE_SIMULATION, value);
}
