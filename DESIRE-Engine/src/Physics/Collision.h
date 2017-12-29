#pragma once

#include "Core/math/vectormath.h"

class PhysicsComponent;

class Collision
{
public:
	static const int MAX_CONTACT_POINTS = 4;

	PhysicsComponent *component;
	PhysicsComponent *otherComponent;

	Vector3 contactPoints[MAX_CONTACT_POINTS];
	Vector3 contactNormals[MAX_CONTACT_POINTS];
	int pointCount;
};
