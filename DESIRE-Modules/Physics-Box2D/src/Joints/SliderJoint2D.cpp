#include "stdafx_Box2D.h"
#include "Joints/SliderJoint2D.h"

#include "b2MathExt.h"

SliderJoint2D::SliderJoint2D()
{
	m_jointDef.userData = this;
}

void SliderJoint2D::SetUseMotor(bool value)
{
	m_jointDef.enableMotor = value;

	if(m_pJoint != nullptr)
	{
		static_cast<b2PrismaticJoint*>(m_pJoint)->EnableMotor(m_jointDef.enableMotor);
	}
}

bool SliderJoint2D::GetUseMotor() const
{
	return m_jointDef.enableMotor;
}

void SliderJoint2D::SetMotor(const Motor& motor)
{
	m_jointDef.maxMotorForce = motor.maxMotorForce;
	m_jointDef.motorSpeed = motor.motorSpeed;

	if(m_pJoint != nullptr)
	{
		static_cast<b2PrismaticJoint*>(m_pJoint)->SetMaxMotorForce(m_jointDef.maxMotorForce);
		static_cast<b2PrismaticJoint*>(m_pJoint)->SetMotorSpeed(m_jointDef.motorSpeed);
	}
}

Joint2D::Motor SliderJoint2D::GetMotor() const
{
	Motor motor;
	motor.maxMotorForce = m_jointDef.maxMotorForce;
	motor.motorSpeed = m_jointDef.motorSpeed;
	return motor;
}

void SliderJoint2D::SetUseLimits(bool value)
{
	m_jointDef.enableLimit = value;

	if(m_pJoint != nullptr)
	{
		static_cast<b2PrismaticJoint*>(m_pJoint)->EnableLimit(m_jointDef.enableLimit);
	}
}

bool SliderJoint2D::GetUseLimits() const
{
	return m_jointDef.enableLimit;
}

void SliderJoint2D::SetLimits(const TranslationLimits& limits)
{
	m_jointDef.lowerTranslation = limits.lowerTranslation;
	m_jointDef.upperTranslation = limits.upperTranslation;

	if(m_pJoint != nullptr)
	{
		static_cast<b2PrismaticJoint*>(m_pJoint)->SetLimits(m_jointDef.lowerTranslation, m_jointDef.upperTranslation);
	}
}

SliderJoint2D::TranslationLimits SliderJoint2D::GetLimits() const
{
	TranslationLimits limits;
	limits.lowerTranslation = m_jointDef.lowerTranslation;
	limits.upperTranslation = m_jointDef.upperTranslation;
	return limits;
}

float SliderJoint2D::GetMotorForce(float timeStep) const
{
	ASSERT(m_pJoint != nullptr && "Only a simulated joint has this property");
	return static_cast<b2PrismaticJoint*>(m_pJoint)->GetMotorForce(timeStep);
}

float SliderJoint2D::GetJointTranslation() const
{
	ASSERT(m_pJoint != nullptr && "Only a simulated joint has this property");
	return static_cast<b2PrismaticJoint*>(m_pJoint)->GetJointTranslation();
}

float SliderJoint2D::GetJointSpeed() const
{
	ASSERT(m_pJoint != nullptr && "Only a simulated joint has this property");
	return static_cast<b2PrismaticJoint*>(m_pJoint)->GetJointSpeed();
}

void SliderJoint2D::CreateJoint()
{
	const b2Vec2 worldAxis(1.0f, 0.0f);
	m_jointDef.localAxisA = m_jointDef.bodyA->GetLocalVector(worldAxis);
	m_jointDef.referenceAngle = m_jointDef.bodyB->GetAngle() - m_jointDef.bodyA->GetAngle();
	m_jointDef.localAnchorA = GetB2Vec2(GetAnchor());
	m_jointDef.localAnchorB = GetB2Vec2(GetConnectedAnchor());

	Joint2D::CreateJoint();
}

b2JointDef& SliderJoint2D::GetJointDef()
{
	return m_jointDef;
}

const b2JointDef& SliderJoint2D::GetJointDef() const
{
	return m_jointDef;
}
