#pragma once

#include "Engine/Physics/ColliderShape.h"

class Box2DColliderShape : public ColliderShape
{
public:
	virtual size_t GetShapeCount() const = 0;
	virtual const b2Shape* GetShape(size_t idx) const = 0;
};
