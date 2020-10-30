#include "stdafx_Box2D.h"
#include "Joints/HingeJoint2D.h"

#include "b2MathExt.h"

#include "Engine/Core/Math/math.h"

HingeJoint2D::HingeJoint2D()
{
	m_jointDef.userData = this;
}

void HingeJoint2D::SetUseMotor(bool value)
{
	m_jointDef.enableMotor = value;

	if(m_pJoint != nullptr)
	{
		static_cast<b2RevoluteJoint*>(m_pJoint)->EnableMotor(m_jointDef.enableMotor);
	}
}

bool HingeJoint2D::GetUseMotor() const
{
	return m_jointDef.enableMotor;
}

void HingeJoint2D::SetMotor(const Joint2D::Motor& motor)
{
	m_jointDef.maxMotorTorque = motor.maxMotorForce;
	m_jointDef.motorSpeed = motor.motorSpeed;

	if(m_pJoint != nullptr)
	{
		static_cast<b2RevoluteJoint*>(m_pJoint)->SetMaxMotorTorque(m_jointDef.maxMotorTorque);
		static_cast<b2RevoluteJoint*>(m_pJoint)->SetMotorSpeed(m_jointDef.motorSpeed);
	}
}

Joint2D::Motor HingeJoint2D::GetMotor() const
{
	Motor motor;
	motor.maxMotorForce = m_jointDef.maxMotorTorque;
	motor.motorSpeed = m_jointDef.motorSpeed;
	return motor;
}

void HingeJoint2D::SetUseLimits(bool value)
{
	m_jointDef.enableLimit = value;

	if(m_pJoint != nullptr)
	{
		static_cast<b2RevoluteJoint*>(m_pJoint)->EnableLimit(m_jointDef.enableLimit);
	}
}

bool HingeJoint2D::GetUseLimits() const
{
	return m_jointDef.enableLimit;
}

void HingeJoint2D::SetLimits(const AngleLimits& limits)
{
	m_jointDef.lowerAngle = Math::DegToRad(limits.lowerAngle);
	m_jointDef.upperAngle = Math::DegToRad(limits.upperAngle);

	if(m_pJoint != nullptr)
	{
		static_cast<b2RevoluteJoint*>(m_pJoint)->SetLimits(m_jointDef.lowerAngle, m_jointDef.upperAngle);
	}
}

HingeJoint2D::AngleLimits HingeJoint2D::GetLimits() const
{
	AngleLimits limits;
	limits.lowerAngle = Math::RadToDeg(m_jointDef.lowerAngle);
	limits.upperAngle = Math::RadToDeg(m_jointDef.upperAngle);
	return limits;
}

float HingeJoint2D::GetMotorTorque(float timeStep) const
{
	ASSERT(m_pJoint != nullptr && "Only a simulated joint has this property");
	return static_cast<b2RevoluteJoint*>(m_pJoint)->GetMotorTorque(timeStep);
}

float HingeJoint2D::GetJointAngle() const
{
	ASSERT(m_pJoint != nullptr && "Only a simulated joint has this property");
	return Math::RadToDeg(static_cast<b2RevoluteJoint*>(m_pJoint)->GetJointAngle());
}

float HingeJoint2D::GetJointSpeed() const
{
	ASSERT(m_pJoint != nullptr && "Only a simulated joint has this property");
	return static_cast<b2RevoluteJoint*>(m_pJoint)->GetJointSpeed();
}

void HingeJoint2D::CreateJoint()
{
	m_jointDef.referenceAngle = m_jointDef.bodyB->GetAngle() - m_jointDef.bodyA->GetAngle();
	m_jointDef.localAnchorA = GetB2Vec2(GetAnchor());
	m_jointDef.localAnchorB = GetB2Vec2(GetConnectedAnchor());

	Joint2D::CreateJoint();
}

b2JointDef& HingeJoint2D::GetJointDef()
{
	return m_jointDef;
}

const b2JointDef& HingeJoint2D::GetJointDef() const
{
	return m_jointDef;
}
