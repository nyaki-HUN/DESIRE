#include "stdafx_Box2D.h"
#include "Joints/SpringJoint2D.h"

#include "b2MathExt.h"

SpringJoint2D::SpringJoint2D()
{
	m_jointDef.userData = this;
	m_jointDef.length = 1.0f;
}

void SpringJoint2D::SetDampingRatio(float value)
{
	m_jointDef.dampingRatio = value;

	if(m_pJoint != nullptr)
	{
		static_cast<b2DistanceJoint*>(m_pJoint)->SetDampingRatio(m_jointDef.dampingRatio);
	}
}

float SpringJoint2D::GetDampingRatio() const
{
	return m_jointDef.dampingRatio;
}

void SpringJoint2D::SetFrequency(float value)
{
	m_jointDef.frequencyHz = value;

	if(m_pJoint != nullptr)
	{
		static_cast<b2DistanceJoint*>(m_pJoint)->SetFrequency(m_jointDef.frequencyHz);
	}
}

float SpringJoint2D::GetFrequency() const
{
	return m_jointDef.frequencyHz;
}

void SpringJoint2D::SetDistance(float value)
{
	m_jointDef.length = value;

	if(m_pJoint != nullptr)
	{
		static_cast<b2DistanceJoint*>(m_pJoint)->SetLength(m_jointDef.length);
	}
}

float SpringJoint2D::GetDistance() const
{
	return m_jointDef.length;
}

void SpringJoint2D::CreateJoint()
{
	m_jointDef.localAnchorA = GetB2Vec2(GetAnchor());
	m_jointDef.localAnchorB = GetB2Vec2(GetConnectedAnchor());

	Joint2D::CreateJoint();
}

b2JointDef& SpringJoint2D::GetJointDef()
{
	return m_jointDef;
}

const b2JointDef& SpringJoint2D::GetJointDef() const
{
	return m_jointDef;
}
