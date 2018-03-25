#pragma once

#include "Engine/Core/math/Vector3.h"

class PhysicsComponent;

class Collision
{
public:
	static const int k_MaxContactPoints = 4;

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

	Vector3 contactPoints[k_MaxContactPoints];
	Vector3 contactNormals[k_MaxContactPoints];
	int pointCount = 0;
};
