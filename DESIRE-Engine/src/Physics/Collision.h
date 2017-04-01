#pragma once

#include "Core/math/vectormath.h"

class PhysicsComponent;

class Collision
{
public:
	static const int MAX_CONTACT_POINTS = 4;

	Vector3 contactPoints[MAX_CONTACT_POINTS];
	Vector3 force[MAX_CONTACT_POINTS];
	PhysicsComponent *otherComponent;
};
