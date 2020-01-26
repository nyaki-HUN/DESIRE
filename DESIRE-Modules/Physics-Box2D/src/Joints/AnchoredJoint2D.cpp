#include "stdafx_Box2D.h"
#include "Joints/AnchoredJoint2D.h"

AnchoredJoint2D::AnchoredJoint2D()
{
}

void AnchoredJoint2D::SetConnectedComponent(Box2DPhysicsComponent* component)
{
	if(anchoredComponent == connectedComponent)
	{
		// Not allowed to connect to self
		return;
	}

	connectedComponent = component;
	CreateJointBetween(anchoredComponent, connectedComponent);
}

Box2DPhysicsComponent* AnchoredJoint2D::GetConnectedComponent() const
{
	return connectedComponent;
}

void AnchoredJoint2D::SetAnchor(const Vector2& value)
{
	anchorInLocalSpace = value;
	CreateJoint();
}

const Vector2& AnchoredJoint2D::GetAnchor() const
{
	return anchorInLocalSpace;
}

void AnchoredJoint2D::SetConnectedAnchor(const Vector2& value)
{
	connectedAnchorInLocalSpace = value;
	CreateJoint();
}

const Vector2& AnchoredJoint2D::GetConnectedAnchor() const
{
	return connectedAnchorInLocalSpace;
}
