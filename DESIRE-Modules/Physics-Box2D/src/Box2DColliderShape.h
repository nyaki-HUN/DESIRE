#pragma once

#include "Engine/Physics/ColliderShape.h"

class Box2DColliderShape : public ColliderShape
{
public:
	virtual int GetShapeCount() const = 0;
	virtual const b2Shape* GetShape(int idx) const = 0;
};
