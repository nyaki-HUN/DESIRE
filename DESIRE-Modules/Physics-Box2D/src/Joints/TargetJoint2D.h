#pragma once

#include "Joints/Joint2D.h"

class TargetJoint2D : public Joint2D
{
public:
	TargetJoint2D();

	void SetAnchor(Vector2 value);
	Vector2 GetAnchor() const;

	void SetDumpingRatio(float value);
	float GetDumpingRatio() const;

	void SetFrequency(float value);
	float GetFrequency() const;

	void SetMaxForce(float value);
	float GetMaxForce() const;

	void SetTarget(Vector2 value);
	Vector2 GetTarget() const;

	void CreateJointOnComponent(Box2DPhysicsComponent* anchoredComponent);

private:
	void CreateJoint() override;

	b2JointDef& GetJointDef() override;
	const b2JointDef& GetJointDef() const override;

	b2MouseJointDef m_jointDef;
	Vector2 m_anchorInLocalSpace = { 0.0f, 0.0f };
};
