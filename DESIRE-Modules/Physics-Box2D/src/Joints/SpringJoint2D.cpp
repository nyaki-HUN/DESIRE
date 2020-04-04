#include "stdafx_Box2D.h"
#include "Joints/SpringJoint2D.h"

#include "b2MathExt.h"

SpringJoint2D::SpringJoint2D()
{
	jointDef.userData = this;
	jointDef.length = 1.0f;
}

void SpringJoint2D::SetDampingRatio(float value)
{
	jointDef.dampingRatio = value;

	if(joint != nullptr)
	{
		static_cast<b2DistanceJoint*>(joint)->SetDampingRatio(jointDef.dampingRatio);
	}
}

float SpringJoint2D::GetDampingRatio() const
{
	return jointDef.dampingRatio;
}

void SpringJoint2D::SetFrequency(float value)
{
	jointDef.frequencyHz = value;

	if(joint != nullptr)
	{
		static_cast<b2DistanceJoint*>(joint)->SetFrequency(jointDef.frequencyHz);
	}
}

float SpringJoint2D::GetFrequency() const
{
	return jointDef.frequencyHz;
}

void SpringJoint2D::SetDistance(float value)
{
	jointDef.length = value;

	if(joint != nullptr)
	{
		static_cast<b2DistanceJoint*>(joint)->SetLength(jointDef.length);
	}
}

float SpringJoint2D::GetDistance() const
{
	return jointDef.length;
}

void SpringJoint2D::CreateJoint()
{
	jointDef.localAnchorA = GetB2Vec2(GetAnchor());
	jointDef.localAnchorB = GetB2Vec2(GetConnectedAnchor());

	Joint2D::CreateJoint();
}

b2JointDef& SpringJoint2D::GetJointDef()
{
	return jointDef;
}

const b2JointDef& SpringJoint2D::GetJointDef() const
{
	return jointDef;
}
