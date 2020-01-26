#pragma once

#include "Joints/AnchoredJoint2D.h"

#include "box2d/b2_distance_joint.h"

class SpringJoint2D : public AnchoredJoint2D
{
public:
	SpringJoint2D();

	void SetDampingRatio(float value);
	float GetDampingRatio() const;

	void SetFrequency(float value);
	float GetFrequency() const;

	void SetDistance(float value);
	float GetDistance() const;

private:
	void CreateJoint() override;

	b2JointDef& GetJointDef() override;
	const b2JointDef& GetJointDef() const override;

	b2DistanceJointDef jointDef;
};
