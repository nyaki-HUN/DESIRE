#include "Joints/AnchoredJoint2D.h"

AnchoredJoint2D::AnchoredJoint2D()
{

}

AnchoredJoint2D::~AnchoredJoint2D()
{

}

const Vector2& AnchoredJoint2D::GetAnchor() const
{
	return anchorInLocalSpace;
}

void AnchoredJoint2D::SetAnchor(const Vector2& value)
{
	anchorInLocalSpace = value;
	RecreateJoint();
}

const Vector2& AnchoredJoint2D::GetConnectedAnchor() const
{
	return connectedAnchorInLocalSpace;
}

void AnchoredJoint2D::SetConnectedAnchor(const Vector2& value)
{
	connectedAnchorInLocalSpace = value;
	RecreateJoint();
}
