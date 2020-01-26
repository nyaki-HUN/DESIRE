#pragma once

#include "Engine/Core/Math/Vector2.h"
#include "Engine/Core/Math/Vector3.h"

#include "box2d/b2_math.h"

inline Vector2 GetVector2(const b2Vec2& vec)
{
	return Vector2(vec.x, vec.y);
}

inline b2Vec2 GetB2Vec2(const Vector2& vec)
{
	return b2Vec2(vec.GetX(), vec.GetY());
}

inline Vector3 GetVector3(const b2Vec2& vec)
{
	return Vector3(vec.x, vec.y, 0.0f);
}

inline b2Vec2 GetB2Vec2(const Vector3& vec)
{
	return b2Vec2(vec.GetX(), vec.GetY());
}
