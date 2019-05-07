#pragma once

#include "Engine/Core/math/Vector3.h"

class PhysicsComponent;

class Collision
{
public:
	static constexpr int kMaxContactPoints = 4;

	Collision()
	{

	}

	// Constructor to create single-point collision
	Collision(PhysicsComponent *component, const Vector3& point, const Vector3& normal)
		: component(component)
		, pointCount(1)
	{
		contactPoints[0] = point;
		contactNormals[0] = normal;
	}

	PhysicsComponent *component = nullptr;
	PhysicsComponent *incomingComponent = nullptr;

	Vector3 contactPoints[kMaxContactPoints];
	Vector3 contactNormals[kMaxContactPoints];
	int pointCount = 0;
};
