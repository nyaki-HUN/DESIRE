#include "Joints/HingeJoint2D.h"
#include "b2MathExt.h"

#include "Engine/Core/math/math.h"

#include "Box2D/Dynamics/b2Body.h"

HingeJoint2D::HingeJoint2D()
{
	jointDef.userData = this;
}

HingeJoint2D::~HingeJoint2D()
{

}

void HingeJoint2D::SetUseMotor(bool value)
{
	jointDef.enableMotor = value;

	if(joint != nullptr)
	{
		static_cast<b2RevoluteJoint*>(joint)->EnableMotor(jointDef.enableMotor);
	}
}

bool HingeJoint2D::GetUseMotor() const
{
	return jointDef.enableMotor;
}

void HingeJoint2D::SetMotor(const Joint2D::Motor& motor)
{
	jointDef.maxMotorTorque = motor.maxMotorForce;
	jointDef.motorSpeed = motor.motorSpeed;

	if(joint != nullptr)
	{
		static_cast<b2RevoluteJoint*>(joint)->SetMaxMotorTorque(jointDef.maxMotorTorque);
		static_cast<b2RevoluteJoint*>(joint)->SetMotorSpeed(jointDef.motorSpeed);
	}
}

Joint2D::Motor HingeJoint2D::GetMotor() const
{
	Motor motor;
	motor.maxMotorForce = jointDef.maxMotorTorque;
	motor.motorSpeed = jointDef.motorSpeed;
	return motor;
}

void HingeJoint2D::SetUseLimits(bool value)
{
	jointDef.enableLimit = value;

	if(joint != nullptr)
	{
		static_cast<b2RevoluteJoint*>(joint)->EnableLimit(jointDef.enableLimit);
	}
}

bool HingeJoint2D::GetUseLimits() const
{
	return jointDef.enableLimit;
}

void HingeJoint2D::SetLimits(const AngleLimits& limits)
{
	jointDef.lowerAngle = Math::DegToRad(limits.lowerAngle);
	jointDef.upperAngle = Math::DegToRad(limits.upperAngle);

	if(joint != nullptr)
	{
		static_cast<b2RevoluteJoint*>(joint)->SetLimits(jointDef.lowerAngle, jointDef.upperAngle);
	}
}

HingeJoint2D::AngleLimits HingeJoint2D::GetLimits() const
{
	AngleLimits limits;
	limits.lowerAngle = Math::RadToDeg(jointDef.lowerAngle);
	limits.upperAngle = Math::RadToDeg(jointDef.upperAngle);
	return limits;
}

float HingeJoint2D::GetMotorTorque(float timeStep) const
{
	ASSERT(joint != nullptr && "Only a simulated joint has this property");
	return static_cast<b2RevoluteJoint*>(joint)->GetMotorTorque(timeStep);
}

float HingeJoint2D::GetJointAngle() const
{
	ASSERT(joint != nullptr && "Only a simulated joint has this property");
	return Math::RadToDeg(static_cast<b2RevoluteJoint*>(joint)->GetJointAngle());
}

float HingeJoint2D::GetJointSpeed() const
{
	ASSERT(joint != nullptr && "Only a simulated joint has this property");
	return static_cast<b2RevoluteJoint*>(joint)->GetJointSpeed();
}

void HingeJoint2D::CreateJoint()
{
	jointDef.referenceAngle = jointDef.bodyB->GetAngle() - jointDef.bodyA->GetAngle();
	jointDef.localAnchorA = GetB2Vec2(GetAnchor());
	jointDef.localAnchorB = GetB2Vec2(GetConnectedAnchor());

	Joint2D::CreateJoint();
}

b2JointDef& HingeJoint2D::GetJointDef()
{
	return jointDef;
}

const b2JointDef& HingeJoint2D::GetJointDef() const
{
	return jointDef;
}
