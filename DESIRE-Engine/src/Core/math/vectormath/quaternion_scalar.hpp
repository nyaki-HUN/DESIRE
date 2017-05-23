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
	float recipSinAngle, scale0, scale1, cosAngle, angle;
	cosAngle = unitQuat0.Dot(unitQuat1);
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
		angle = acosf(cosAngle);
		recipSinAngle = (1.0f / sinf(angle));
		scale0 = (sinf(((1.0f - t) * angle)) * recipSinAngle);
		scale1 = (sinf((t * angle)) * recipSinAngle);
	}
	else
	{
		scale0 = (1.0f - t);
		scale1 = t;
	}
	return start * scale0 + unitQuat1 * scale1;
}

DESIRE_FORCE_INLINE Quat Quat::CreateRotation(const Vector3& unitVec0, const Vector3& unitVec1)
{
	float cosHalfAngleX2, recipCosHalfAngleX2;
	cosHalfAngleX2 = sqrtf((2.0f * (1.0f + unitVec0.Dot(unitVec1))));
	recipCosHalfAngleX2 = (1.0f / cosHalfAngleX2);
	return Quat((unitVec0.Cross(unitVec1) * recipCosHalfAngleX2), (cosHalfAngleX2 * 0.5f));
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
	const float t0 = std::cos(halfAngle.mVec128.z);
	const float t1 = std::sin(halfAngle.mVec128.z);
	const float t2 = std::cos(halfAngle.mVec128.x);
	const float t3 = std::sin(halfAngle.mVec128.x);
	const float t4 = std::cos(halfAngle.mVec128.y);
	const float t5 = std::sin(halfAngle.mVec128.y);
	return Quat(
		t0 * t3 * t4 - t1 * t2 * t5,
		t0 * t2 * t5 + t1 * t3 * t4,
		t1 * t2 * t4 - t0 * t3 * t5,
		t0 * t2 * t4 + t1 * t3 * t5
	);
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
