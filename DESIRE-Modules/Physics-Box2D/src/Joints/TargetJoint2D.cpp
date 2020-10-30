#include "stdafx_Box2D.h"
#include "Joints/TargetJoint2D.h"

#include "b2MathExt.h"
#include "Box2DPhysics.h"
#include "Box2DPhysicsComponent.h"

TargetJoint2D::TargetJoint2D()
{
	m_jointDef.userData = this;
}

void TargetJoint2D::SetAnchor(Vector2 value)
{
	m_anchorInLocalSpace = value;
	CreateJoint();
}

Vector2 TargetJoint2D::GetAnchor() const
{
	return m_anchorInLocalSpace;
}

void TargetJoint2D::SetDumpingRatio(float value)
{
	m_jointDef.dampingRatio = value;

	if(m_pJoint != nullptr)
	{
		static_cast<b2MouseJoint*>(m_pJoint)->SetDampingRatio(m_jointDef.dampingRatio);
	}
}

float TargetJoint2D::GetDumpingRatio() const
{
	return m_jointDef.dampingRatio;
}

void TargetJoint2D::SetFrequency(float value)
{
	m_jointDef.frequencyHz = value;

	if(m_pJoint != nullptr)
	{
		static_cast<b2MouseJoint*>(m_pJoint)->SetFrequency(m_jointDef.frequencyHz);
	}
}

float TargetJoint2D::GetFrequency() const
{
	return m_jointDef.frequencyHz;
}

void TargetJoint2D::SetMaxForce(float value)
{
	m_jointDef.maxForce = value;

	if(m_pJoint != nullptr)
	{
		static_cast<b2MouseJoint*>(m_pJoint)->SetMaxForce(m_jointDef.maxForce);
	}
}

float TargetJoint2D::GetMaxForce() const
{
	return m_jointDef.maxForce;
}

void TargetJoint2D::SetTarget(Vector2 value)
{
	m_jointDef.target = GetB2Vec2(value);

	if(m_pJoint != nullptr)
	{
		static_cast<b2MouseJoint*>(m_pJoint)->SetTarget(m_jointDef.target);
	}
}

Vector2 TargetJoint2D::GetTarget() const
{
	b2Vec2 target = (m_pJoint != nullptr) ? static_cast<b2MouseJoint*>(m_pJoint)->GetTarget() : m_jointDef.target;
	Vector2 result = GetVector2(target);
	return result;
}

void TargetJoint2D::CreateJointOnComponent(Box2DPhysicsComponent* anchoredComponent)
{
	m_jointDef.bodyA = static_cast<Box2DPhysics*>(Modules::Physics.get())->GetWorldBody();
	// bodyB is the body what we want to move
	m_jointDef.bodyB = anchoredComponent->GetBody();
	CreateJoint();
}

void TargetJoint2D::CreateJoint()
{
	// Anchor is giving in local space of the rigid body where the joint is connected to.
	// So
	// -bodyA would be the mouse, but it is not exists.
	// -bodyB is the body where the Anchor set up in local space. In order to setup this, we have to set the target temporary to the world position of the anchor. 
	//  (According to the manual: "The target point initially coincides with the body’s anchor point")

	b2Vec2 userDefinedTarget = m_jointDef.target;
	b2Vec2 localAnchorOnB = GetB2Vec2(GetAnchor());
	m_jointDef.target = b2Mul(m_jointDef.bodyB->GetTransform(), localAnchorOnB);

	Joint2D::CreateJoint();

	// Set the target which was defined by the user
	static_cast<b2MouseJoint*>(m_pJoint)->SetTarget(userDefinedTarget);
}

b2JointDef& TargetJoint2D::GetJointDef()
{
	return m_jointDef;
}

const b2JointDef& TargetJoint2D::GetJointDef() const
{
	return m_jointDef;
}
