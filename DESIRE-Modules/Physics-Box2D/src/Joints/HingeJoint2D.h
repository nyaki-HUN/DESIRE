#pragma once

#include "Joints/AnchoredJoint2D.h"

class HingeJoint2D : public AnchoredJoint2D
{
public:
	struct AngleLimits
	{
		float lowerAngle;
		float upperAngle;
	};

	HingeJoint2D();

	void SetUseMotor(bool value);
	bool GetUseMotor() const;

	void SetMotor(const Motor& motor);
	Motor GetMotor() const;

	void SetUseLimits(bool value);
	bool GetUseLimits() const;

	void SetLimits(const AngleLimits& value);
	AngleLimits GetLimits() const;

	float GetMotorTorque(float timeStep) const;
	float GetJointAngle() const;
	float GetJointSpeed() const;

private:
	void CreateJoint() override;

	b2JointDef& GetJointDef() override;
	const b2JointDef& GetJointDef() const override;

	b2RevoluteJointDef m_jointDef;
};
