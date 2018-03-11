#pragma once

#include "Engine/Core/math/Vector2.h"

#include "Box2D/Common/b2Math.h"

inline Vector2 GetVector2(const b2Vec2& vec)
{
	return Vector2(vec.x, vec.y);
}

inline b2Vec2 GetB2Vec2(const Vector2& vec)
{
	return b2Vec2(vec.GetX(), vec.GetY());
}
