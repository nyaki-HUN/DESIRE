#pragma once

#include "Engine/Core/math/Vector3.h"
#include "Engine/Core/math/Quat.h"

#include "foundation/PxVec3.h"
#include "foundation/PxQuat.h"

inline Vector3 GetVector3(const physx::PxVec3& pxVec)
{
	return Vector3(pxVec.x, pxVec.y, pxVec.z);
}

inline physx::PxVec3 GetPxVec3(const Vector3& vec)
{
	return physx::PxVec3(vec.GetX(), vec.GetY(), vec.GetZ());
}

inline Quat GetQuat(const physx::PxQuat& pxQuat)
{
	return Quat(pxQuat.x, pxQuat.y, pxQuat.z, pxQuat.w);
}

inline physx::PxQuat GetPxQuat(const Quat& quat)
{
	return physx::PxQuat(quat.GetX(), quat.GetY(), quat.GetZ(), quat.GetW());
}
