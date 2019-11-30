#pragma once

#include "Engine/Core/Math/Vector3.h"
#include "Engine/Core/Math/Quat.h"

#include "LinearMath/btVector3.h"
#include "LinearMath/btQuaternion.h"

#if (defined(DESIRE_USE_SSE) && defined(BT_USE_SSE)) || (defined(__ARM_NEON__) && defined(BT_USE_NEON))
	#define USE_NATIVE_CONVERSION
#endif

inline Vector3 GetVector3(const btVector3& bulletVec)
{
#if defined(USE_NATIVE_CONVERSION)
	return Vector3(bulletVec.get128());
#else
	btVector3FloatData data;
	bulletVec.serializeFloat(data);

	Vector3 vec;
	vec.LoadXYZ(data.m_floats);
	return vec;
#endif
}

inline btVector3 GetBtVector3(const Vector3& vec)
{
#if defined(USE_NATIVE_CONVERSION)
	return btVector3(vec);
#else
	btVector3FloatData data = {};
	vec.StoreXYZ(data.m_floats);

	btVector3 bulletVec;
	bulletVec.deSerializeFloat(data);
	return bulletVec;
#endif
}

inline Quat GetQuat(const btQuaternion& bulletQuat)
{
#if defined(USE_NATIVE_CONVERSION)
	return Quat(bulletQuat.get128());
#else
	btQuaternionFloatData data;
	bulletQuat.serializeFloat(data);

	Quat quat;
	quat.LoadXYZW(data.m_floats);
	return quat;
#endif
}

inline btQuaternion GetBtQuat(const Quat& quat)
{
#if defined(USE_NATIVE_CONVERSION)
	return btQuaternion(quat);
#else
	btQuaternionFloatData data;
	quat.StoreXYZW(data.m_floats);

	btQuaternion bulletQuat;
	bulletQuat.deSerializeFloat(data);
	return bulletQuat;
#endif
}

#undef USE_NATIVE_CONVERSION
