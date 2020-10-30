#include "stdafx_Box2D.h"
#include "Joints/AnchoredJoint2D.h"

AnchoredJoint2D::AnchoredJoint2D()
{
}

void AnchoredJoint2D::SetConnectedComponent(Box2DPhysicsComponent* pPhysicsComponent)
{
	if(m_pAnchoredComponent == m_pConnectedComponent)
	{
		// Not allowed to connect to self
		return;
	}

	m_pConnectedComponent = pPhysicsComponent;
	CreateJointBetween(m_pAnchoredComponent, m_pConnectedComponent);
}

Box2DPhysicsComponent* AnchoredJoint2D::GetConnectedComponent() const
{
	return m_pConnectedComponent;
}

void AnchoredJoint2D::SetAnchor(const Vector2& value)
{
	m_anchorInLocalSpace = value;
	CreateJoint();
}

const Vector2& AnchoredJoint2D::GetAnchor() const
{
	return m_anchorInLocalSpace;
}

void AnchoredJoint2D::SetConnectedAnchor(const Vector2& value)
{
	m_connectedAnchorInLocalSpace = value;
	CreateJoint();
}

const Vector2& AnchoredJoint2D::GetConnectedAnchor() const
{
	return m_connectedAnchorInLocalSpace;
}
