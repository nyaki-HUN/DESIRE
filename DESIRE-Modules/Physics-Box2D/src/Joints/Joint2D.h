#pragma once

#include "Engine/Core/Math/Vector2.h"

class Box2DPhysicsComponent;
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

	void SetCollisionEnabled(bool enabled);
	bool IsCollisionEnabled() const;

	Vector2 GetReactionForce(float timeStep) const;
	float GetReactionTorque(float timeStep) const;

	void CreateJointBetween(Box2DPhysicsComponent *anchoredComponent, Box2DPhysicsComponent *connectedComponent);

	// This function will be called when a Box2D joint is destroyed (usually when a connecected RigidBody is destroyed).
	// Make sure 'jointDef.userData' is properly set to 'this' in the derived class.
	void OnJointDestroyed();

protected:
	virtual void CreateJoint();
	void DestroyJoint();

	virtual b2JointDef& GetJointDef() = 0;
	virtual const b2JointDef& GetJointDef() const = 0;

	b2Joint *joint = nullptr;
};
