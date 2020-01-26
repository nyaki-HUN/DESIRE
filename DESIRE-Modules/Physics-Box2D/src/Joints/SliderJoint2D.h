#pragma once

#include "Joints/AnchoredJoint2D.h"

#include "box2d/b2_prismatic_joint.h"

class SliderJoint2D : public AnchoredJoint2D
{
public:
	struct TranslationLimits
	{
		float lowerTranslation;
		float upperTranslation;
	};

	SliderJoint2D();

	void SetUseMotor(bool value);
	bool GetUseMotor() const;

	void SetMotor(const Motor& motor);
	Motor GetMotor() const;

	void SetUseLimits(bool value);
	bool GetUseLimits() const;

	void SetLimits(const TranslationLimits& value);
	TranslationLimits GetLimits() const;

	float GetMotorForce(float timeStep) const;
	float GetJointTranslation() const;
	float GetJointSpeed() const;

private:
	void CreateJoint() override;

	b2JointDef& GetJointDef() override;
	const b2JointDef& GetJointDef() const override;

	b2PrismaticJointDef jointDef;
};
