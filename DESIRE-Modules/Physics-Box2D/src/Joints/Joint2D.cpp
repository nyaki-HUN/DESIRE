#include "stdafx_Box2D.h"
#include "Joints/Joint2D.h"

#include "b2MathExt.h"
#include "Box2DPhysics.h"
#include "Box2DPhysicsComponent.h"

Joint2D::Joint2D()
{
}

Joint2D::~Joint2D()
{
	DestroyJoint();
}

void Joint2D::SetCollisionEnabled(bool value)
{
	GetJointDef().collideConnected = value;
	CreateJoint();
}

bool Joint2D::IsCollisionEnabled() const
{
	return GetJointDef().collideConnected;
}

Vector2 Joint2D::GetReactionForce(float timeStep) const
{
	ASSERT(m_pJoint != nullptr);
	return GetVector2(m_pJoint->GetReactionForce(timeStep));
}

float Joint2D::GetReactionTorque(float timeStep) const
{
	ASSERT(m_pJoint != nullptr);
	return m_pJoint->GetReactionTorque(timeStep);
}

void Joint2D::CreateJointBetween(Box2DPhysicsComponent* pAnchoredComponent, Box2DPhysicsComponent* pConnectedComponent)
{
	if(pAnchoredComponent == nullptr)
	{
		return;
	}

	GetJointDef().bodyA = pAnchoredComponent->GetBody();
	GetJointDef().bodyB = (pConnectedComponent != nullptr) ? pConnectedComponent->GetBody() : static_cast<Box2DPhysics&>(*Modules::Physics).GetWorldBody();
	CreateJoint();
}

void Joint2D::OnJointDestroyed()
{
	m_pJoint = nullptr;
}

void Joint2D::CreateJoint()
{
	DestroyJoint();

	const b2JointDef& jointDef = GetJointDef();
	if(jointDef.bodyA == nullptr || jointDef.bodyB == nullptr)
	{
		return;
	}

	b2World& world = static_cast<Box2DPhysics&>(*Modules::Physics).GetWorld();
	m_pJoint = world.CreateJoint(&jointDef);
}

void Joint2D::DestroyJoint()
{
	if(m_pJoint != nullptr)
	{
		b2World& world = static_cast<Box2DPhysics&>(*Modules::Physics).GetWorld();
		world.DestroyJoint(m_pJoint);
		m_pJoint = nullptr;
	}
}
