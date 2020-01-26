#include "stdafx_Box2D.h"
#include "Joints/SliderJoint2D.h"
#include "b2MathExt.h"

SliderJoint2D::SliderJoint2D()
{
	jointDef.userData = this;
}

void SliderJoint2D::SetUseMotor(bool value)
{
	jointDef.enableMotor = value;

	if(joint != nullptr)
	{
		static_cast<b2PrismaticJoint*>(joint)->EnableMotor(jointDef.enableMotor);
	}
}

bool SliderJoint2D::GetUseMotor() const
{
	return jointDef.enableMotor;
}

void SliderJoint2D::SetMotor(const Motor& motor)
{
	jointDef.maxMotorForce = motor.maxMotorForce;
	jointDef.motorSpeed = motor.motorSpeed;

	if(joint != nullptr)
	{
		static_cast<b2PrismaticJoint*>(joint)->SetMaxMotorForce(jointDef.maxMotorForce);
		static_cast<b2PrismaticJoint*>(joint)->SetMotorSpeed(jointDef.motorSpeed);
	}
}

Joint2D::Motor SliderJoint2D::GetMotor() const
{
	Motor motor;
	motor.maxMotorForce = jointDef.maxMotorForce;
	motor.motorSpeed = jointDef.motorSpeed;
	return motor;
}

void SliderJoint2D::SetUseLimits(bool value)
{
	jointDef.enableLimit = value;

	if(joint != nullptr)
	{
		static_cast<b2PrismaticJoint*>(joint)->EnableLimit(jointDef.enableLimit);
	}
}

bool SliderJoint2D::GetUseLimits() const
{
	return jointDef.enableLimit;
}

void SliderJoint2D::SetLimits(const TranslationLimits& limits)
{
	jointDef.lowerTranslation = limits.lowerTranslation;
	jointDef.upperTranslation = limits.upperTranslation;

	if(joint != nullptr)
	{
		static_cast<b2PrismaticJoint*>(joint)->SetLimits(jointDef.lowerTranslation, jointDef.upperTranslation);
	}
}

SliderJoint2D::TranslationLimits SliderJoint2D::GetLimits() const
{
	TranslationLimits limits;
	limits.lowerTranslation = jointDef.lowerTranslation;
	limits.upperTranslation = jointDef.upperTranslation;
	return limits;
}

float SliderJoint2D::GetMotorForce(float timeStep) const
{
	ASSERT(joint != nullptr && "Only a simulated joint has this property");
	return static_cast<b2PrismaticJoint*>(joint)->GetMotorForce(timeStep);
}

float SliderJoint2D::GetJointTranslation() const
{
	ASSERT(joint != nullptr && "Only a simulated joint has this property");
	return static_cast<b2PrismaticJoint*>(joint)->GetJointTranslation();
}

float SliderJoint2D::GetJointSpeed() const
{
	ASSERT(joint != nullptr && "Only a simulated joint has this property");
	return static_cast<b2PrismaticJoint*>(joint)->GetJointSpeed();
}

void SliderJoint2D::CreateJoint()
{
	const b2Vec2 worldAxis(1.0f, 0.0f);
	jointDef.localAxisA = jointDef.bodyA->GetLocalVector(worldAxis);
	jointDef.referenceAngle = jointDef.bodyB->GetAngle() - jointDef.bodyA->GetAngle();
	jointDef.localAnchorA = GetB2Vec2(GetAnchor());
	jointDef.localAnchorB = GetB2Vec2(GetConnectedAnchor());

	Joint2D::CreateJoint();
}

b2JointDef& SliderJoint2D::GetJointDef()
{
	return jointDef;
}

const b2JointDef& SliderJoint2D::GetJointDef() const
{
	return jointDef;
}
