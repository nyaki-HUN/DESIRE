#pragma once

class Matrix3;

class Quat
{
public:
	inline Quat()
	{
		// No initialization
	}

	inline Quat(const Quat& vec)
		: mVec128(vec.mVec128)
	{

	}
	
	inline Quat(vec_float4_t vf4)
		: mVec128(vf4)
	{

	}
	
	inline Quat(float x, float y, float z, float w)
	{
		mVec128 = SIMD::Construct(x, y, z, w);
	}

	// Convert a rotation matrix to a unit-length quaternion
	explicit inline Quat(const Matrix3& rotMat);

	// Load x, y, z, and w elements from the first four words of a float array
	inline void LoadXYZW(const float *fptr)
	{
		SIMD::LoadXYZW(mVec128, fptr);
	}

	// Store x, y, z, and w elements of a quaternion in the first four words of a float array
	inline void StoreXYZW(float *fptr) const
	{
		SIMD::StoreXYZW(mVec128, fptr);
	}

	// Set element
	inline void SetX(float x)		{ SIMD::SetX(mVec128, x); }
	inline void SetY(float y)		{ SIMD::SetY(mVec128, y); }
	inline void SetZ(float z)		{ SIMD::SetZ(mVec128, z); }
	inline void SetW(float w)		{ SIMD::SetW(mVec128, w); }

	// Get element
	inline float GetX() const		{ return SIMD::GetX(mVec128); }
	inline float GetY() const		{ return SIMD::GetY(mVec128); }
	inline float GetZ() const		{ return SIMD::GetZ(mVec128); }
	inline float GetW() const		{ return SIMD::GetW(mVec128); }

	// Operator overloads
	inline operator vec_float4_t() const
	{
		return mVec128;
	}

	inline Quat& operator =(const Quat& quat)
	{
		mVec128 = quat.mVec128;
		return *this;
	}

	inline Quat operator -() const						{ return SIMD::Negate(mVec128); }
	inline Quat operator +(const Quat& quat) const		{ return SIMD::Add(mVec128, quat.mVec128); }
	inline Quat operator -(const Quat& quat) const		{ return SIMD::Sub(mVec128, quat.mVec128); }

	inline Quat operator *(const Quat& quat) const;

	inline Quat& operator +=(const Quat& quat)
	{
		*this = *this + quat;
		return *this;
	}

	inline Quat& operator -=(const Quat& quat)
	{
		*this = *this - quat;
		return *this;
	}

	inline Quat& operator *=(const Quat& quat)
	{
		*this = *this * quat;
		return *this;
	}

	// Compute the dot product of two quaternions
	inline float Dot(const Quat& quat) const
	{
		return SIMD::GetX(SIMD::Dot4(mVec128, quat));
	}

	// Compute the norm of a quaternion
	inline float Norm() const
	{
		return Dot(*this);
	}

	// Compute the length of a quaternion
	inline float Length() const
	{
		return std::sqrt(Norm());
	}

	// Compute the conjugate of a quaternion
	inline Quat Conjugate() const;

	// Compute the Euler angle representation of the rotation in radians
	inline Vector3 EulerAngles() const
	{
		const vec_float4_t vecSq2 = SIMD::Mul(SIMD::Mul(mVec128, mVec128), 2.0f);
		const float tmpX1 = 1.0f - (SIMD::GetX(vecSq2) + SIMD::GetY(vecSq2));
		const float tmpX2 = 1.0f - (SIMD::GetY(vecSq2) + SIMD::GetZ(vecSq2));

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

	// Rotate a 3-D vector with this unit-length quaternion
	inline Vector3 RotateVec(const Vector3& vec) const;

	// Normalize a quaternion
	// NOTE: The result is unpredictable when all elements of quat are at or near zero.
	inline void Normalize()
	{
		mVec128 = SIMD::Mul(mVec128, newtonrapson_rsqrt4(SIMD::Dot4(mVec128, mVec128)));
	}

	// Get normalized quaternion
	// NOTE: The result is unpredictable when all elements of quat are at or near zero.
	inline Quat Normalized() const
	{
		return SIMD::Mul(mVec128, newtonrapson_rsqrt4(SIMD::Dot4(mVec128, mVec128)));
	}

	// Spherical linear interpolation between two quaternions
	// NOTE: 
	// Interpolates along the shortest path between orientations
	// Does not clamp t between 0 and 1.
	static inline Quat Slerp(float t, const Quat& unitQuat0, const Quat& unitQuat1);

	// Spherical quadrangle interpolation
	static inline Quat Squad(float t, const Quat& unitQuat0, const Quat& unitQuat1, const Quat& unitQuat2, const Quat& unitQuat3)
	{
		return Quat::Slerp((2.0f * t) * (1.0f - t), Quat::Slerp(t, unitQuat0, unitQuat3), Quat::Slerp(t, unitQuat1, unitQuat2));
	}

	// Construct an identity quaternion
	static inline Quat Identity()
	{
		return Quat(0.0f, 0.0f, 0.0f, 1.0f);
	}

	// Construct a quaternion to rotate around a unit-length 3-D vector
	static inline Quat CreateRotation(float radians, const Vector3& unitVec);

	// Construct a quaternion to rotate around the x axis
	static inline Quat CreateRotationX(float radians);

	// Construct a quaternion to rotate around the y axis
	static inline Quat CreateRotationY(float radians);

	// Construct a quaternion to rotate around the z axis
	static inline Quat CreateRotationZ(float radians);

	// Construct a quaternion to rotate using Euler angles for each axis
	static inline Quat CreateRotationFromEulerAngles(const Vector3& radiansXYZ);

	// Construct a quaternion to rotate between two unit-length 3-D vectors
	// NOTE: The result is unpredictable if unitVec0 and unitVec1 point in opposite directions.
	static inline Quat CreateRotationFromTo(const Vector3& unitVecFrom, const Vector3& unitVecTo);

private:
	vec_float4_t mVec128;
};

// --------------------------------------------------------------------------------------------------------------------

// Multiply two quaternions
inline Quat Quat::operator *(const Quat& quat) const
{
#if defined(DESIRE_USE_SSE)
	const __m128 tmp0 = SIMD::Swizzle_YZXW(mVec128);
	const __m128 tmp1 = SIMD::Swizzle_ZXYW(quat.mVec128);
	const __m128 tmp2 = SIMD::Swizzle_ZXYW(mVec128);
	const __m128 tmp3 = SIMD::Swizzle_YZXW(quat.mVec128);
	__m128 qv = SIMD::Mul(SIMD::Swizzle_WWWW(mVec128), quat.mVec128);
	qv = SIMD::MulAdd(SIMD::Swizzle_WWWW(quat.mVec128), mVec128, qv);
	qv = SIMD::MulAdd(tmp0, tmp1, qv);
	qv = SIMD::MulSub(tmp2, tmp3, qv);
	const __m128 product = SIMD::Mul(mVec128, quat.mVec128);
	const __m128 l_wxyz = _mm_ror_ps(mVec128, 3);
	const __m128 r_wxyz = _mm_ror_ps(quat.mVec128, 3);
	__m128 qw = SIMD::MulSub(l_wxyz, r_wxyz, product);
	const __m128 xy = SIMD::MulAdd(l_wxyz, r_wxyz, product);
	qw = SIMD::Sub(qw, _mm_ror_ps(xy, 2));
	return SIMD::Blend_W(qv, qw);
#else
	return Quat(
		((GetW() * quat.GetX() + GetX() * quat.GetW()) + GetY() * quat.GetZ()) - GetZ() * quat.GetY(),
		((GetW() * quat.GetY() + GetY() * quat.GetW()) + GetZ() * quat.GetX()) - GetX() * quat.GetZ(),
		((GetW() * quat.GetZ() + GetZ() * quat.GetW()) + GetX() * quat.GetY()) - GetY() * quat.GetX(),
		((GetW() * quat.GetW() - GetX() * quat.GetX()) - GetY() * quat.GetY()) - GetZ() * quat.GetZ()
	);
#endif
}

// Compute the conjugate of a quaternion
inline Quat Quat::Conjugate() const
{
#if defined(DESIRE_USE_SSE)
	alignas(16) const uint32_t mask[4] = { 0x80000000, 0x80000000, 0x80000000, 0 };
	return _mm_xor_ps(mVec128, _mm_load_ps((float*)mask));
#elif defined(__ARM_NEON__)
	const uint32x4_t mask = vdupq_n_u32(0x80000000);
	mask = vsetq_lane_u32(0, mask, 3);
	return vreinterpretq_f32_u32(veorq_u32(vreinterpretq_u32_f32(mVec128), mask));
#else
	return Quat(-mVec128.x, -mVec128.y, -mVec128.z, mVec128.w);
#endif
}

// Rotate a 3-D vector with this unit-length quaternion
inline Vector3 Quat::RotateVec(const Vector3& vec) const
{
#if defined(DESIRE_USE_SSE)
	const __m128 tmp0 = SIMD::Swizzle_YZXW(mVec128);
	__m128 tmp1 = SIMD::Swizzle_ZXYW(vec);
	const __m128 tmp2 = SIMD::Swizzle_ZXYW(mVec128);
	__m128 tmp3 = SIMD::Swizzle_YZXW(vec);
	const __m128 wwww = SIMD::Swizzle_WWWW(mVec128);
	__m128 qv = SIMD::Mul(wwww, vec);
	qv = SIMD::MulAdd(tmp0, tmp1, qv);
	qv = SIMD::MulSub(tmp2, tmp3, qv);
	const __m128 product = SIMD::Mul(mVec128, vec);
	__m128 qw = SIMD::MulAdd(_mm_ror_ps(mVec128, 1), _mm_ror_ps(vec, 1), product);
	qw = SIMD::Add(_mm_ror_ps(product, 2), qw);
	tmp1 = SIMD::Swizzle_ZXYW(qv);
	tmp3 = SIMD::Swizzle_YZXW(qv);
	__m128 res = SIMD::Mul(SIMD::Swizzle_XXXX(qw), mVec128);
	res = SIMD::MulAdd(wwww, qv, res);
	res = SIMD::MulAdd(tmp0, tmp1, res);
	res = SIMD::MulSub(tmp2, tmp3, res);
	return res;
#else
	const float tmpX = ((GetW() * vec.GetX()) + (GetY() * vec.GetZ())) - (GetZ() * vec.GetY());
	const float tmpY = ((GetW() * vec.GetY()) + (GetZ() * vec.GetX())) - (GetX() * vec.GetZ());
	const float tmpZ = ((GetW() * vec.GetZ()) + (GetX() * vec.GetY())) - (GetY() * vec.GetX());
	const float tmpW = ((GetX() * vec.GetX()) + (GetY() * vec.GetY())) + (GetZ() * vec.GetZ());
	return Vector3(
		(((tmpW * GetX()) + (tmpX * GetW())) - (tmpY * GetZ())) + (tmpZ * GetY()),
		(((tmpW * GetY()) + (tmpY * GetW())) - (tmpZ * GetX())) + (tmpX * GetZ()),
		(((tmpW * GetZ()) + (tmpZ * GetW())) - (tmpX * GetY())) + (tmpY * GetX())
	);
#endif
}

// Spherical linear interpolation between two quaternions
inline Quat Quat::Slerp(float t, const Quat& unitQuat0, const Quat& unitQuat1)
{
#if defined(DESIRE_USE_SSE)
	__m128 cosAngle = SIMD::Dot4(unitQuat0, unitQuat1);
	__m128 selectMask = _mm_cmpgt_ps(_mm_setzero_ps(), cosAngle);
	cosAngle = SIMD::Blend(cosAngle, SIMD::Negate(cosAngle), selectMask);
	const __m128 start = SIMD::Blend(unitQuat0, SIMD::Negate(unitQuat0), selectMask);
	selectMask = _mm_cmpgt_ps(_mm_set1_ps(_VECTORMATH_SLERP_TOL), cosAngle);
	const __m128 angle = acosf4(cosAngle);
	const __m128 tttt = _mm_set1_ps(t);
	const __m128 oneMinusT = _mm_sub_ps(_mm_set1_ps(1.0f), tttt);
	__m128 angles = _mm_unpacklo_ps(_mm_set1_ps(1.0f), tttt);
	angles = _mm_unpacklo_ps(angles, oneMinusT);
	angles = SIMD::MulAdd(angles, angle, _mm_setzero_ps());
	const __m128 sines = sinf4(angles);
	const __m128 scales = _mm_div_ps(sines, SIMD::Swizzle_XXXX(sines));
	const __m128 scale0 = SIMD::Blend(oneMinusT, SIMD::Swizzle_YYYY(scales), selectMask);
	const __m128 scale1 = SIMD::Blend(tttt, SIMD::Swizzle_ZZZZ(scales), selectMask);
	return SIMD::MulAdd(start, scale0, SIMD::Mul(unitQuat1, scale1));
#else
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
#endif
}

// Construct a quaternion to rotate around a unit-length 3-D vector
inline Quat Quat::CreateRotation(float radians, const Vector3& unitVec)
{
	const float halfAngle = radians * 0.5f;

#if defined(DESIRE_USE_SSE)
	__m128 s, c;
	sincosf4(_mm_set1_ps(halfAngle), &s, &c);
#else
	const float s = std::sin(halfAngle);
	const Vector4 c(std::cos(halfAngle));
#endif
	return SIMD::Blend_W(SIMD::Mul(unitVec, s), c);
}

// Construct a quaternion to rotate around the x axis
inline Quat Quat::CreateRotationX(float radians)
{
	const float halfAngle = radians * 0.5f;

#if defined(DESIRE_USE_SSE)
	__m128 s, c;
	sincosf4(_mm_set1_ps(halfAngle), &s, &c);
	const __m128 res = SIMD::Blend_X(_mm_setzero_ps(), s);
	return SIMD::Blend_W(res, c);
#else
	const float s = std::sin(halfAngle);
	const float c = std::cos(halfAngle);
	return Quat(s, 0.0f, 0.0f, c);
#endif
}

// Construct a quaternion to rotate around the y axis
inline Quat Quat::CreateRotationY(float radians)
{
	const float halfAngle = radians * 0.5f;

#if defined(DESIRE_USE_SSE)
	__m128 s, c;
	sincosf4(_mm_set1_ps(halfAngle), &s, &c);
	const __m128 res = SIMD::Blend_Y(_mm_setzero_ps(), s);
	return SIMD::Blend_W(res, c);
#else
	const float s = std::sin(halfAngle);
	const float c = std::cos(halfAngle);
	return Quat(0.0f, s, 0.0f, c);
#endif
}

// Construct a quaternion to rotate around the z axis
inline Quat Quat::CreateRotationZ(float radians)
{
	const float halfAngle = radians * 0.5f;

#if defined(DESIRE_USE_SSE)
	__m128 s, c;
	sincosf4(_mm_set1_ps(halfAngle), &s, &c);
	const __m128 res = SIMD::Blend_Z(_mm_setzero_ps(), s);
	return SIMD::Blend_W(res, c);
#else
	const float s = std::sin(halfAngle);
	const float c = std::cos(halfAngle);
	return Quat(0.0f, 0.0f, s, c);
#endif
}

// Construct a quaternion to rotate using Euler angles for each axis
inline Quat Quat::CreateRotationFromEulerAngles(const Vector3& radiansXYZ)
{
#if defined(DESIRE_USE_SSE)
	__m128 s, c;
	sincosf4(radiansXYZ * 0.5f, &s, &c);
	const float cZ = SIMD::GetZ(c);
	const float sZ = SIMD::GetZ(s);
	const float cX = SIMD::GetX(c);
	const float sX = SIMD::GetX(s);
	const float cY = SIMD::GetY(c);
	const float sY = SIMD::GetY(s);
#else
	const Vector3 halfAngle = radiansXYZ * 0.5f;
	const float cZ = std::cos(halfAngle.GetZ());
	const float sZ = std::sin(halfAngle.GetZ());
	const float cX = std::cos(halfAngle.GetX());
	const float sX = std::sin(halfAngle.GetX());
	const float cY = std::cos(halfAngle.GetY());
	const float sY = std::sin(halfAngle.GetY());
#endif

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

// Construct a quaternion to rotate between two unit-length 3-D vectors
inline Quat Quat::CreateRotationFromTo(const Vector3& unitVecFrom, const Vector3& unitVecTo)
{
	Quat result;

#if defined(DESIRE_USE_SSE)
	const __m128 cosAngle = SIMD::Dot3(unitVecFrom, unitVecTo);
	const __m128 cosAngleX2Plus2 = SIMD::MulAdd(cosAngle, _mm_set1_ps(2.0f), _mm_set1_ps(2.0f));
	const __m128 recipCosHalfAngleX2 = _mm_rsqrt_ps(cosAngleX2Plus2);
	const __m128 cosHalfAngleX2 = SIMD::Mul(recipCosHalfAngleX2, cosAngleX2Plus2);
	const __m128 res = SIMD::Mul(unitVecFrom.Cross(unitVecTo), recipCosHalfAngleX2);
	result = SIMD::Blend_W(res, SIMD::Mul(cosHalfAngleX2, 0.5f));
#else
	const float cosHalfAngleX2 = std::sqrt((2.0f * (1.0f + unitVecFrom.Dot(unitVecTo))));
	const float recipCosHalfAngleX2 = (1.0f / cosHalfAngleX2);
	result.mVec128 = SIMD::Mul(unitVecFrom.Cross(unitVecTo), recipCosHalfAngleX2);
	result.SetW(cosHalfAngleX2 * 0.5f);
#endif

	return result;
}
