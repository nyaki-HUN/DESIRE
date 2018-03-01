#pragma once

#include "Engine/Core/math/Vector3.h"

class PhysicsComponent;

class Collision
{
public:
	static const int MAX_CONTACT_POINTS = 4;

	PhysicsComponent *component = nullptr;
	PhysicsComponent *incomingComponent = nullptr;

	Vector3 contactPoints[MAX_CONTACT_POINTS];
	Vector3 contactNormals[MAX_CONTACT_POINTS];
	int pointCount = 0;
};
