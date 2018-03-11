#pragma once

#include "Joints/Joint2D.h"

class Box2DPhysicsComponent;

class AnchoredJoint2D : public Joint2D
{
protected:
	AnchoredJoint2D();

public:
	~AnchoredJoint2D() override;

	const Vector2& GetAnchor() const;
	void SetAnchor(const Vector2& value);

	const Vector2& GetConnectedAnchor() const;
	void SetConnectedAnchor(const Vector2& value);

	virtual void CreateJointBetweenBodies(Box2DPhysicsComponent *anchoredComponent, Box2DPhysicsComponent *connectedComponent) = 0;

private:
	Vector2 anchorInLocalSpace = { 0.0f, 0.0f };
	Vector2 connectedAnchorInLocalSpace = { 0.0f, 0.0f };
};
