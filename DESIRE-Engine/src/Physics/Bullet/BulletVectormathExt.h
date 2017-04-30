#pragma once

#include "Core/math/vectormath.h"

#include "Physics-Bullet/src/LinearMath/btVector3.h"
#include "Physics-Bullet/src/LinearMath/btQuaternion.h"

#if (defined(DESIRE_USE_SSE) && defined(BT_USE_SSE)) || (defined(__ARM_NEON__) && defined(BT_USE_NEON))
	#define BULLET_VECTORMATH_USE_VEC128	1
#else
	#define BULLET_VECTORMATH_USE_VEC128	0
#endif

inline Vector3 GetVector3(const btVector3& bulletVec)
{
#if BULLET_VECTORMATH_USE_VEC128
	return Vector3(bulletVec.get128());
#else
	return Vector3((float)bulletVec.getX(), (float)bulletVec.getY(), (float)bulletVec.getZ());
#endif
}

inline btVector3 GetBtVector3(const Vector3& vec)
{
#if BULLET_VECTORMATH_USE_VEC128
	return btVector3(vec);
#else
	return btVector3(vec.GetX(), vec.GetY(), vec.GetZ());
#endif
}

inline Quat GetQuat(const btQuaternion& bulletQuat)
{
#if BULLET_VECTORMATH_USE_VEC128
	return Quat(bulletQuat.get128());
#else
	return Quat((float)bulletQuat.getX(), (float)bulletQuat.getY(), (float)bulletQuat.getZ(), (float)bulletQuat.getW());
#endif
}

inline btQuaternion GetBtQuat(const Quat& quat)
{
#if BULLET_VECTORMATH_USE_VEC128
	return btQuaternion(quat);
#else
	return btQuaternion(quat.GetX(), quat.GetY(), quat.GetZ(), quat.GetW());
#endif
}
