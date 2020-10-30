#include "stdafx_PhysX.h"
#include "PhysXPhysicsComponent.h"

#include "PhysXPhysics.h"
#include "PxMathExt.h"

#include "Engine/Core/Math/Transform.h"
#include "Engine/Core/Object.h"

PhysXPhysicsComponent::PhysXPhysicsComponent(Object& object)
	: PhysicsComponent(object)
{
}

PhysXPhysicsComponent::~PhysXPhysicsComponent()
{
}

void PhysXPhysicsComponent::SetEnabled(bool value)
{
	if(IsEnabled() == value)
	{
		return;
	}

	PhysicsComponent::SetEnabled(value);
	m_pBody->setActorFlag(physx::PxActorFlag::eDISABLE_SIMULATION, value);
}

void PhysXPhysicsComponent::SetCollisionLayer(EPhysicsCollisionLayer collisionLayer)
{
	m_collisionLayer = collisionLayer;

	physx::PxFilterData filterData;
	filterData.word0 = 1 << static_cast<int>(collisionLayer);
	filterData.word1 = Modules::Physics->GetMaskForCollisionLayer(collisionLayer);
}

void PhysXPhysicsComponent::SetCollisionDetectionMode(ECollisionDetectionMode mode)
{
	m_pBody->setRigidBodyFlag(physx::PxRigidBodyFlag::eENABLE_CCD, (mode == ECollisionDetectionMode::Continuous));
}

PhysicsComponent::ECollisionDetectionMode PhysXPhysicsComponent::GetCollisionDetectionMode() const
{
	return (m_pBody->getRigidBodyFlags() & physx::PxRigidBodyFlag::eENABLE_CCD) ? ECollisionDetectionMode::Continuous : ECollisionDetectionMode::Discrete;
}

Array<PhysicsComponent*> PhysXPhysicsComponent::GetActiveCollidingComponents() const
{
	Array<PhysicsComponent*> collisions;
	ASSERT(false && "TODO");
	return collisions;
}

void PhysXPhysicsComponent::SetBodyType(EBodyType m_pBodyType)
{
	switch(m_pBodyType)
	{
		case EBodyType::Static:		ASSERT(false && "TODO");  break;
		case EBodyType::Dynamic:	m_pBody->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, false); break;
		case EBodyType::Kinematic:	m_pBody->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, true); break;
	}
}

PhysicsComponent::EBodyType PhysXPhysicsComponent::GetBodyType() const
{
	const physx::PxRigidBodyFlags flags = m_pBody->getRigidBodyFlags();
	if(flags.isSet(physx::PxRigidBodyFlag::eKINEMATIC))
	{
		return PhysicsComponent::EBodyType::Kinematic;
	}

	if(m_pDynamicBody != nullptr)
	{
		PhysicsComponent::EBodyType::Dynamic;
	}

	return PhysicsComponent::EBodyType::Static;
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
	return m_pBody->setMass(mass);
}

float PhysXPhysicsComponent::GetMass() const
{
	return m_pBody->getMass();
}

Vector3 PhysXPhysicsComponent::GetCenterOfMass() const
{
	m_pBody->getCMassLocalPose();
	ASSERT(false && "TODO");
	return Vector3::Zero();
}

void PhysXPhysicsComponent::SetLinearDamping(float value)
{
	if(m_pDynamicBody != nullptr)
	{
		m_pDynamicBody->setLinearDamping(value);
	}
}

float PhysXPhysicsComponent::GetLinearDamping() const
{
	return (m_pDynamicBody != nullptr) ? m_pDynamicBody->getLinearDamping() : 0.0f;
}

void PhysXPhysicsComponent::SetAngularDamping(float value)
{
	if(m_pDynamicBody != nullptr)
	{
		m_pDynamicBody->setAngularDamping(value);
	}
}

float PhysXPhysicsComponent::GetAngularDamping() const
{
	return (m_pDynamicBody != nullptr) ? m_pDynamicBody->getAngularDamping() : 0.0f;
}

void PhysXPhysicsComponent::SetLinearVelocity(const Vector3& linearVelocity)
{
	m_pBody->setLinearVelocity(GetPxVec3(linearVelocity));
}

Vector3 PhysXPhysicsComponent::GetLinearVelocity() const
{
	return GetVector3(m_pBody->getLinearVelocity());
}

void PhysXPhysicsComponent::SetAngularVelocity(const Vector3& angularVelocity)
{
	m_pBody->setAngularVelocity(GetPxVec3(angularVelocity));
}

Vector3 PhysXPhysicsComponent::GetAngularVelocity() const
{
	return GetVector3(m_pBody->getAngularVelocity());
}

void PhysXPhysicsComponent::AddForce(const Vector3& force, EForceMode mode)
{
	switch(mode)
	{
		case EForceMode::Force:		m_pBody->addForce(GetPxVec3(force), physx::PxForceMode::eFORCE); break;
		case EForceMode::Impulse:	m_pBody->addForce(GetPxVec3(force), physx::PxForceMode::eIMPULSE); break;
	}
}

void PhysXPhysicsComponent::AddForceAtPosition(const Vector3& force, const Vector3& position, EForceMode mode)
{
	switch(mode)
	{
		case EForceMode::Force:		physx::PxRigidBodyExt::addForceAtPos(*m_pBody, GetPxVec3(force), GetPxVec3(position), physx::PxForceMode::eFORCE); break;
		case EForceMode::Impulse:	physx::PxRigidBodyExt::addForceAtPos(*m_pBody, GetPxVec3(force), GetPxVec3(position), physx::PxForceMode::eIMPULSE); break;
	}
}

void PhysXPhysicsComponent::AddTorque(const Vector3& torque, EForceMode mode)
{
	switch(mode)
	{
		case EForceMode::Force:		m_pBody->addTorque(GetPxVec3(torque), physx::PxForceMode::eFORCE); break;
		case EForceMode::Impulse:	m_pBody->addTorque(GetPxVec3(torque), physx::PxForceMode::eIMPULSE); break;
	}
}

void PhysXPhysicsComponent::SetLinearMotionLock(bool axisX, bool axisY, bool axisZ)
{
	if(m_pDynamicBody != nullptr)
	{
		m_pDynamicBody->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_X, axisX);
		m_pDynamicBody->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Y, axisY);
		m_pDynamicBody->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Z, axisZ);
	}
}

void PhysXPhysicsComponent::SetAngularMotionLock(bool axisX, bool axisY, bool axisZ)
{
	if(m_pDynamicBody != nullptr)
	{
		m_pDynamicBody->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, axisX);
		m_pDynamicBody->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, axisY);
		m_pDynamicBody->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, axisZ);
	}
}

bool PhysXPhysicsComponent::IsSleeping() const
{
	return (m_pDynamicBody != nullptr) ? m_pDynamicBody->isSleeping() : true;
}

void PhysXPhysicsComponent::UpdateGameObjectTransform() const
{
	Transform& transform = object.GetTransform();
	const physx::PxTransform pxTransform = m_pBody->getGlobalPose();
	transform.SetPosition(GetVector3(pxTransform.p));
	transform.SetRotation(GetQuat(pxTransform.q));
}

void PhysXPhysicsComponent::SetTransformFromGameObject()
{
	const Transform& transform = object.GetTransform();
	physx::PxTransform pxTransform = m_pBody->getGlobalPose();
	pxTransform.p = GetPxVec3(transform.GetPosition());
	pxTransform.q = GetPxQuat(transform.GetRotation());
	m_pBody->setGlobalPose(pxTransform);
}
