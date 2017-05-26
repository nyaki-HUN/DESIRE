// --------------------------------------------------------------------------------------------------------------------
//	Scalar implementation of quaternion class functions
// --------------------------------------------------------------------------------------------------------------------

DESIRE_FORCE_INLINE Vector3 Quat::RotateVec(const Vector3& vec) const
{
	const float tmpX = ((GetW() * vec.GetX()) + (GetY() * vec.GetZ())) - (GetZ() * vec.GetY());
	const float tmpY = ((GetW() * vec.GetY()) + (GetZ() * vec.GetX())) - (GetX() * vec.GetZ());
	const float tmpZ = ((GetW() * vec.GetZ()) + (GetX() * vec.GetY())) - (GetY() * vec.GetX());
	const float tmpW = ((GetX() * vec.GetX()) + (GetY() * vec.GetY())) + (GetZ() * vec.GetZ());
	return Vector3(
		(((tmpW * GetX()) + (tmpX * GetW())) - (tmpY * GetZ())) + (tmpZ * GetY()),
		(((tmpW * GetY()) + (tmpY * GetW())) - (tmpZ * GetX())) + (tmpX * GetZ()),
		(((tmpW * GetZ()) + (tmpZ * GetW())) - (tmpX * GetY())) + (tmpY * GetX())
	);
}

DESIRE_FORCE_INLINE Quat Quat::Slerp(float t, const Quat& unitQuat0, const Quat& unitQuat1)
{
	Quat start;
	float scale0, scale1;
	float cosAngle = unitQuat0.Dot(unitQuat1);
	if(cosAngle < 0.0f)
	{
		cosAngle = -cosAngle;
		start = (-unitQuat0);
	}
	else
	{
		start = unitQuat0;
	}

	if(cosAngle < _VECTORMATH_SLERP_TOL)
	{
		const float angle = std::acos(cosAngle);
		const float recipSinAngle = (1.0f / std::sin(angle));
		scale0 = (std::sin(((1.0f - t) * angle)) * recipSinAngle);
		scale1 = (std::sin((t * angle)) * recipSinAngle);
	}
	else
	{
		scale0 = (1.0f - t);
		scale1 = t;
	}

	return start * scale0 + unitQuat1 * scale1;
}

DESIRE_FORCE_INLINE Quat Quat::CreateRotation(float radians, const Vector3& unitVec)
{
	const float angle = radians * 0.5f;
	const float s = std::sin(angle);
	const float c = std::cos(angle);
	return Quat((unitVec * s), c);
}

DESIRE_FORCE_INLINE Quat Quat::CreateRotationX(float radians)
{
	const float angle = radians * 0.5f;
	const float s = std::sin(angle);
	const float c = std::cos(angle);
	return Quat(s, 0.0f, 0.0f, c);
}

DESIRE_FORCE_INLINE Quat Quat::CreateRotationY(float radians)
{
	const float angle = radians * 0.5f;
	const float s = std::sin(angle);
	const float c = std::cos(angle);
	return Quat(0.0f, s, 0.0f, c);
}

DESIRE_FORCE_INLINE Quat Quat::CreateRotationZ(float radians)
{
	const float angle = radians * 0.5f;
	const float s = std::sin(angle);
	const float c = std::cos(angle);
	return Quat(0.0f, 0.0f, s, c);
}

DESIRE_FORCE_INLINE Quat Quat::CreateRotationFromEulerAngles(const Vector3& radiansXYZ)
{
	const Vector3 halfAngle = radiansXYZ * 0.5f;
	const float cZ = std::cos(halfAngle.mVec128.z);
	const float sZ = std::sin(halfAngle.mVec128.z);
	const float cX = std::cos(halfAngle.mVec128.x);
	const float sX = std::sin(halfAngle.mVec128.x);
	const float cY = std::cos(halfAngle.mVec128.y);
	const float sY = std::sin(halfAngle.mVec128.y);

	const float cYcZ = cY * cZ;
	const float sYcZ = sY * cZ;
	const float cYsZ = cY * sZ;
	const float sYsZ = sY * sZ;

	return Quat(
		sX * cYcZ - cX * sYsZ,
		cX * sYcZ + sX * cYsZ,
		cX * cYsZ - sX * sYcZ,
		cX * cYcZ + sX * sYsZ
	);
}

DESIRE_FORCE_INLINE Quat Quat::CreateRotationFromTo(const Vector3& unitVecFrom, const Vector3& unitVecTo)
{
	const float cosHalfAngleX2 = sqrtf((2.0f * (1.0f + unitVecFrom.Dot(unitVecTo))));
	const float recipCosHalfAngleX2 = (1.0f / cosHalfAngleX2);
	return Quat((unitVecFrom.Cross(unitVecTo) * recipCosHalfAngleX2), (cosHalfAngleX2 * 0.5f));
}

DESIRE_FORCE_INLINE Quat Quat::operator *(const Quat& quat) const
{
	return Quat(
		(((mVec128.w * quat.mVec128.x) + (mVec128.x * quat.mVec128.w)) + (mVec128.y * quat.mVec128.z)) - (mVec128.z * quat.mVec128.y),
		(((mVec128.w * quat.mVec128.y) + (mVec128.y * quat.mVec128.w)) + (mVec128.z * quat.mVec128.x)) - (mVec128.x * quat.mVec128.z),
		(((mVec128.w * quat.mVec128.z) + (mVec128.z * quat.mVec128.w)) + (mVec128.x * quat.mVec128.y)) - (mVec128.y * quat.mVec128.x),
		(((mVec128.w * quat.mVec128.w) - (mVec128.x * quat.mVec128.x)) - (mVec128.y * quat.mVec128.y)) - (mVec128.z * quat.mVec128.z)
	);
}
