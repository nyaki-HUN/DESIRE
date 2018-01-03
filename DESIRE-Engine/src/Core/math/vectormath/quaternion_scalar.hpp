// --------------------------------------------------------------------------------------------------------------------
//	Scalar implementation of quaternion class functions
// --------------------------------------------------------------------------------------------------------------------

DESIRE_FORCE_INLINE Vector3 Quat::EulerAngles() const
{
	const Vector4 vecSq2 = Vector4(SIMD::MulPerElem(mVec128, mVec128)) * 2.0f;
	const float tmpX1 = 1.0f - (vecSq2.GetX() + vecSq2.GetY());
	const float tmpX2 = 1.0f - (vecSq2.GetY() + vecSq2.GetZ());

	const float x = GetX();
	const float y = GetY();
	const float z = GetZ();
	const float w = GetW();
	const float t1 = 2.0f * (w * x + y * z);
	const float t2 = 2.0f * (w * y - z * x);
	const float t3 = 2.0f * (w * z + x * y);

	return Vector3(
		std::atan2(t1, tmpX1),
		std::asin(t2 < -1.0f ? -1.0f : (t2 > 1.0f ? 1.0f : t2)),
		std::atan2(t3, tmpX2)
	);
}

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
	const float cZ = std::cos(halfAngle.GetZ());
	const float sZ = std::sin(halfAngle.GetZ());
	const float cX = std::cos(halfAngle.GetX());
	const float sX = std::sin(halfAngle.GetX());
	const float cY = std::cos(halfAngle.GetY());
	const float sY = std::sin(halfAngle.GetY());

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
		((GetW() * quat.GetX() + GetX() * quat.GetW()) + GetY() * quat.GetZ()) - GetZ() * quat.GetY(),
		((GetW() * quat.GetY() + GetY() * quat.GetW()) + GetZ() * quat.GetX()) - GetX() * quat.GetZ(),
		((GetW() * quat.GetZ() + GetZ() * quat.GetW()) + GetX() * quat.GetY()) - GetY() * quat.GetX(),
		((GetW() * quat.GetW() - GetX() * quat.GetX()) - GetY() * quat.GetY()) - GetZ() * quat.GetZ()
	);
}
