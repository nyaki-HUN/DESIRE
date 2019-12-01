#include "Joints/TargetJoint2D.h"
#include "Box2DPhysics.h"
#include "Box2DPhysicsComponent.h"
#include "b2MathExt.h"

#include "Engine/Modules.h"

#include "Engine/Core/assert.h"

#include "Box2D/Dynamics/b2Body.h"

TargetJoint2D::TargetJoint2D()
{
	jointDef.userData = this;
}

TargetJoint2D::~TargetJoint2D()
{

}

void TargetJoint2D::SetAnchor(Vector2 value)
{
	anchorInLocalSpace = value;
	CreateJoint();
}

Vector2 TargetJoint2D::GetAnchor() const
{
	return anchorInLocalSpace;
}

void TargetJoint2D::SetDumpingRatio(float value)
{
	jointDef.dampingRatio = value;

	if(joint != nullptr)
	{
		static_cast<b2MouseJoint*>(joint)->SetDampingRatio(jointDef.dampingRatio);
	}
}

float TargetJoint2D::GetDumpingRatio() const
{
	return jointDef.dampingRatio;
}

void TargetJoint2D::SetFrequency(float value)
{
	jointDef.frequencyHz = value;

	if(joint != nullptr)
	{
		static_cast<b2MouseJoint*>(joint)->SetFrequency(jointDef.frequencyHz);
	}
}

float TargetJoint2D::GetFrequency() const
{
	return jointDef.frequencyHz;
}

void TargetJoint2D::SetMaxForce(float value)
{
	jointDef.maxForce = value;

	if(joint != nullptr)
	{
		static_cast<b2MouseJoint*>(joint)->SetMaxForce(jointDef.maxForce);
	}
}

float TargetJoint2D::GetMaxForce() const
{
	return jointDef.maxForce;
}

void TargetJoint2D::SetTarget(Vector2 value)
{
	jointDef.target = GetB2Vec2(value);

	if(joint != nullptr)
	{
		static_cast<b2MouseJoint*>(joint)->SetTarget(jointDef.target);
	}
}

Vector2 TargetJoint2D::GetTarget() const
{
	b2Vec2 target = (joint != nullptr) ? static_cast<b2MouseJoint*>(joint)->GetTarget() : jointDef.target;
	Vector2 result = GetVector2(target);
	return result;
}

void TargetJoint2D::CreateJointOnComponent(Box2DPhysicsComponent *anchoredComponent)
{
	jointDef.bodyA = static_cast<Box2DPhysics*>(Modules::Physics.get())->GetWorldBody();
	// bodyB is the body what we want to move
	jointDef.bodyB = anchoredComponent->GetBody();
	CreateJoint();
}

void TargetJoint2D::CreateJoint()
{
	// Anchor is giving in local space of the rigid body where the joint is connected to.
	// So
	// -bodyA would be the mouse, but it is not exists.
	// -bodyB is the body where the Anchor set up in local space. In order to setup this, we have to set the target temporary to the world position of the anchor. 
	//  (According to the manual: "The target point initially coincides with the body’s anchor point")

	b2Vec2 userDefinedTarget = jointDef.target;
	b2Vec2 localAnchorOnB = GetB2Vec2(GetAnchor());
	jointDef.target = b2Mul(jointDef.bodyB->GetTransform(), localAnchorOnB);

	Joint2D::CreateJoint();

	// Set the target which was defined by the user
	static_cast<b2MouseJoint*>(joint)->SetTarget(userDefinedTarget);
}

b2JointDef& TargetJoint2D::GetJointDef()
{
	return jointDef;
}

const b2JointDef& TargetJoint2D::GetJointDef() const
{
	return jointDef;
}
