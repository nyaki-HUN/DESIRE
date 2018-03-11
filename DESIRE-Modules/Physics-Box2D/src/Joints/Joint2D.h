#pragma once

#include "Engine/Core/math/Vector2.h"

class b2Joint;
struct b2JointDef;

class Joint2D
{
protected:
	Joint2D();

public:
	struct Motor
	{
		float maxMotorForce;
		float motorSpeed;
	};

	virtual ~Joint2D();

	bool IsCollisionEnabled() const;
	void SetCollisionEnabled(bool enabled);

	Vector2 GetReactionForce(float timeStep) const;
	float GetReactionTorque(float timeStep) const;

	virtual void OnJointDestroyed() = 0;

protected:
	b2Joint* CreateJoint();
	void DestroyJoint();

	virtual b2Joint* GetJoint() const = 0;
	virtual b2JointDef& GetJointDef() = 0;
	virtual const b2JointDef& GetJointDef() const = 0;

	virtual void RecreateJoint() = 0;
};
