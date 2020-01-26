#pragma once

#include "Joints/Joint2D.h"

class AnchoredJoint2D : public Joint2D
{
protected:
	AnchoredJoint2D();

public:
	void SetConnectedComponent(Box2DPhysicsComponent* component);
	Box2DPhysicsComponent* GetConnectedComponent() const;

	void SetAnchor(const Vector2& value);
	const Vector2& GetAnchor() const;

	void SetConnectedAnchor(const Vector2& value);
	const Vector2& GetConnectedAnchor() const;

private:
	Box2DPhysicsComponent* anchoredComponent = nullptr;
	Box2DPhysicsComponent* connectedComponent = nullptr;

	Vector2 anchorInLocalSpace = { 0.0f, 0.0f };
	Vector2 connectedAnchorInLocalSpace = { 0.0f, 0.0f };
};
