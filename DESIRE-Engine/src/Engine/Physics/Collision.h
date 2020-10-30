#pragma once

#include "Engine/Core/Math/Vector3.h"

class PhysicsComponent;

struct Collision
{
	static constexpr int kMaxContactPoints = 4;

	PhysicsComponent* pComponent = nullptr;
	PhysicsComponent* pIncomingComponent = nullptr;

	Vector3 contactPoints[kMaxContactPoints];
	Vector3 contactNormals[kMaxContactPoints];
	int pointCount = 0;

	Collision() = default;

	// Constructor to create single-point collision
	Collision(PhysicsComponent* pComponent, const Vector3& point, const Vector3& normal)
		: pComponent(pComponent)
		, pointCount(1)
	{
		contactPoints[0] = point;
		contactNormals[0] = normal;
	}
};
