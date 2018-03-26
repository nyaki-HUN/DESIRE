#include "Joints/SpringJoint2D.h"
#include "b2MathExt.h"

SpringJoint2D::SpringJoint2D()
{
	jointDef.userData = this;
	jointDef.length = 1.0f;
}

SpringJoint2D::~SpringJoint2D()
{

}

float SpringJoint2D::GetDampingRatio() const
{
	return jointDef.dampingRatio;
}

void SpringJoint2D::SetDampingRatio(float value)
{
	jointDef.dampingRatio = value;

	if(joint != nullptr)
	{
		joint->SetDampingRatio(jointDef.dampingRatio);
	}
}

float SpringJoint2D::GetFrequency() const
{
	return jointDef.frequencyHz;
}

void SpringJoint2D::SetFrequency(float value)
{
	jointDef.frequencyHz = value;

	if(joint != nullptr)
	{
		joint->SetFrequency(jointDef.frequencyHz);
	}
}

float SpringJoint2D::GetDistance() const
{
	return jointDef.length;
}

void SpringJoint2D::SetDistance(float value)
{
	jointDef.length = value;

	if(joint != nullptr)
	{
		joint->SetLength(jointDef.length);
	}
}

void SpringJoint2D::OnJointDestroyed()
{
	joint = nullptr;
}

b2Joint* SpringJoint2D::CreateJoint()
{
	jointDef.localAnchorA = GetB2Vec2(GetAnchor());
	jointDef.localAnchorB = GetB2Vec2(GetConnectedAnchor());

	joint = static_cast<b2DistanceJoint*>(Joint2D::CreateJoint());
	return joint;
}

b2Joint* SpringJoint2D::GetJoint() const
{
	return joint;
}

b2JointDef& SpringJoint2D::GetJointDef()
{
	return jointDef;
}

const b2JointDef& SpringJoint2D::GetJointDef() const
{
	return jointDef;
}
