#pragma once

class Matrix3;

class Quat
{
public:
	DESIRE_FORCE_INLINE Quat()
	{
		// No initialization
	}

	DESIRE_FORCE_INLINE Quat(const Quat& vec)
		: mVec128(vec.mVec128)
	{

	}
	
	DESIRE_FORCE_INLINE Quat(vec_float4_t vf4)
		: mVec128(vf4)
	{

	}
	
	DESIRE_FORCE_INLINE Quat(float x, float y, float z, float w)
	{
		SIMD::Construct(mVec128, x, y, z, w);
	}

	// Convert a rotation matrix to a unit-length quaternion
	explicit DESIRE_FORCE_INLINE Quat(const Matrix3& rotMat);

	// Load x, y, z, and w elements from the first four words of a float array
	DESIRE_FORCE_INLINE void LoadXYZW(const float *fptr)
	{
		SIMD::LoadXYZW(mVec128, fptr);
	}

	// Store x, y, z, and w elements of a quaternion in the first four words of a float array
	DESIRE_FORCE_INLINE void StoreXYZW(float *fptr) const
	{
		SIMD::StoreXYZW(mVec128, fptr);
	}

	// Set element
	DESIRE_FORCE_INLINE void SetX(float x)		{ SIMD::SetX(mVec128, x); }
	DESIRE_FORCE_INLINE void SetY(float y)		{ SIMD::SetY(mVec128, y); }
	DESIRE_FORCE_INLINE void SetZ(float z)		{ SIMD::SetZ(mVec128, z); }
	DESIRE_FORCE_INLINE void SetW(float w)		{ SIMD::SetW(mVec128, w); }

	// Get element
	DESIRE_FORCE_INLINE float GetX() const		{ return SIMD::GetX(mVec128); }
	DESIRE_FORCE_INLINE float GetY() const		{ return SIMD::GetY(mVec128); }
	DESIRE_FORCE_INLINE float GetZ() const		{ return SIMD::GetZ(mVec128); }
	DESIRE_FORCE_INLINE float GetW() const		{ return SIMD::GetW(mVec128); }

	// Operator overloads
	DESIRE_FORCE_INLINE operator vec_float4_t() const
	{
		return mVec128;
	}

	DESIRE_FORCE_INLINE Quat& operator =(const Quat& quat)
	{
		mVec128 = quat.mVec128;
		return *this;
	}

	DESIRE_FORCE_INLINE Quat operator -() const						{ return SIMD::Negate(mVec128); }
	DESIRE_FORCE_INLINE Quat operator +(const Quat& quat) const		{ return SIMD::Add(mVec128, quat.mVec128); }
	DESIRE_FORCE_INLINE Quat operator -(const Quat& quat) const		{ return SIMD::Sub(mVec128, quat.mVec128); }
	DESIRE_FORCE_INLINE Quat operator *(float scalar) const			{ return SIMD::Mul(mVec128, scalar); }

	DESIRE_FORCE_INLINE Quat operator *(const Quat& quat) const
	{
#if defined(DESIRE_USE_SSE)
		const __m128 tmp0 = SIMD::Swizzle_YZXW(mVec128);
		const __m128 tmp1 = SIMD::Swizzle_ZXYW(quat.mVec128);
		const __m128 tmp2 = SIMD::Swizzle_ZXYW(mVec128);
		const __m128 tmp3 = SIMD::Swizzle_YZXW(quat.mVec128);
		__m128 qv = SIMD::Mul(SIMD::Swizzle_WWWW(mVec128), quat.mVec128);
		qv = vec_madd(SIMD::Swizzle_WWWW(quat.mVec128), mVec128, qv);
		qv = vec_madd(tmp0, tmp1, qv);
		qv = vec_nmsub(tmp2, tmp3, qv);
		const __m128 product = SIMD::Mul(mVec128, quat.mVec128);
		const __m128 l_wxyz = _mm_ror_ps(mVec128, 3);
		const __m128 r_wxyz = _mm_ror_ps(quat.mVec128, 3);
		__m128 qw = vec_nmsub(l_wxyz, r_wxyz, product);
		const __m128 xy = vec_madd(l_wxyz, r_wxyz, product);
		qw = SIMD::Sub(qw, _mm_ror_ps(xy, 2));
		alignas(16) const uint32_t select_w[4] = { 0, 0, 0, 0xffffffff };
		const __m128 mask_w = _mm_load_ps((float*)select_w);
		return SIMD::Blend(qv, qw, mask_w);
#else
		return Quat(
			((GetW() * quat.GetX() + GetX() * quat.GetW()) + GetY() * quat.GetZ()) - GetZ() * quat.GetY(),
			((GetW() * quat.GetY() + GetY() * quat.GetW()) + GetZ() * quat.GetX()) - GetX() * quat.GetZ(),
			((GetW() * quat.GetZ() + GetZ() * quat.GetW()) + GetX() * quat.GetY()) - GetY() * quat.GetX(),
			((GetW() * quat.GetW() - GetX() * quat.GetX()) - GetY() * quat.GetY()) - GetZ() * quat.GetZ()
		);
#endif
	}

	DESIRE_FORCE_INLINE Quat operator /(float scalar) const
	{
		return (*this * (1.0f / scalar));
	}

	DESIRE_FORCE_INLINE Quat& operator +=(const Quat& quat)
	{
		*this = *this + quat;
		return *this;
	}

	DESIRE_FORCE_INLINE Quat& operator -=(const Quat& quat)
	{
		*this = *this - quat;
		return *this;
	}

	DESIRE_FORCE_INLINE Quat& operator *=(float scalar)
	{
		*this = *this * scalar;
		return *this;
	}

	DESIRE_FORCE_INLINE Quat& operator *=(const Quat& quat)
	{
		*this = *this * quat;
		return *this;
	}

	DESIRE_FORCE_INLINE Quat& operator /=(float scalar)
	{
		*this = *this / scalar;
		return *this;
	}

	// Compute the dot product of two quaternions
	DESIRE_FORCE_INLINE float Dot(const Quat& quat) const
	{
		return SIMD::GetX(SIMD::Dot4(mVec128, quat));
	}

	// Compute the norm of a quaternion
	DESIRE_FORCE_INLINE float Norm() const
	{
		return Dot(*this);
	}

	// Compute the length of a quaternion
	DESIRE_FORCE_INLINE float Length() const
	{
		return sqrtf(Norm());
	}

	// Compute the conjugate of a quaternion
	DESIRE_FORCE_INLINE Quat Conjugate() const
	{
#if defined(DESIRE_USE_SSE)
		alignas(16) const uint32_t mask[4] = { 0x80000000, 0x80000000, 0x80000000, 0 };
		return _mm_xor_ps(mVec128, _mm_load_ps((float*)mask));
#elif defined(__ARM_NEON__)
		const uint32x4_t mask = vdupq_n_u32(0x80000000);
		mask = vsetq_lane_u32(0, mask, 3);
		return vreinterpretq_f32_u32(veorq_u32(vreinterpretq_u32_f32(mVec128), mask));
#else
		return Quat(-quat.x, -quat.y, -quat.z, quat.w);
#endif
	}

	// Compute the Euler angle representation of the rotation in radians
	DESIRE_FORCE_INLINE Vector3 EulerAngles() const
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
	DESIRE_FORCE_INLINE Vector3 RotateVec(const Vector3& vec) const
	{
#if defined(DESIRE_USE_SSE)
		const __m128 tmp0 = SIMD::Swizzle_YZXW(mVec128);
		__m128 tmp1 = SIMD::Swizzle_ZXYW(vec);
		const __m128 tmp2 = SIMD::Swizzle_ZXYW(mVec128);
		__m128 tmp3 = SIMD::Swizzle_YZXW(vec);
		const __m128 wwww = SIMD::Swizzle_WWWW(mVec128);
		__m128 qv = _mm_mul_ps(wwww, vec);
		qv = vec_madd(tmp0, tmp1, qv);
		qv = vec_nmsub(tmp2, tmp3, qv);
		const __m128 product = _mm_mul_ps(mVec128, vec);
		__m128 qw = vec_madd(_mm_ror_ps(mVec128, 1), _mm_ror_ps(vec, 1), product);
		qw = _mm_add_ps(_mm_ror_ps(product, 2), qw);
		tmp1 = SIMD::Swizzle_ZXYW(qv);
		tmp3 = SIMD::Swizzle_YZXW(qv);
		__m128 res = _mm_mul_ps(SIMD::Swizzle_XXXX(qw), mVec128);
		res = vec_madd(wwww, qv, res);
		res = vec_madd(tmp0, tmp1, res);
		res = vec_nmsub(tmp2, tmp3, res);
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

	// Normalize a quaternion
	// NOTE: The result is unpredictable when all elements of quat are at or near zero.
	DESIRE_FORCE_INLINE void Normalize()
	{
		mVec128 = SIMD::Mul(mVec128, newtonrapson_rsqrt4(SIMD::Dot4(mVec128, mVec128)));
	}

	// Get normalized quaternion
	// NOTE: The result is unpredictable when all elements of quat are at or near zero.
	DESIRE_FORCE_INLINE Quat Normalized() const
	{
		return SIMD::Mul(mVec128, newtonrapson_rsqrt4(SIMD::Dot4(mVec128, mVec128)));
	}

	// Spherical linear interpolation between two quaternions
	// NOTE: 
	// Interpolates along the shortest path between orientations
	// Does not clamp t between 0 and 1.
	static DESIRE_FORCE_INLINE Quat Slerp(float t, const Quat& unitQuat0, const Quat& unitQuat1)
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
		angles = vec_madd(angles, angle, _mm_setzero_ps());
		const __m128 sines = sinf4(angles);
		const __m128 scales = _mm_div_ps(sines, SIMD::Swizzle_XXXX(sines));
		const __m128 scale0 = SIMD::Blend(oneMinusT, SIMD::Swizzle_YYYY(scales), selectMask);
		const __m128 scale1 = SIMD::Blend(tttt, SIMD::Swizzle_ZZZZ(scales), selectMask);
		return vec_madd(start, scale0, _mm_mul_ps(unitQuat1, scale1));
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


	// Spherical quadrangle interpolation
	static DESIRE_FORCE_INLINE Quat Squad(float t, const Quat& unitQuat0, const Quat& unitQuat1, const Quat& unitQuat2, const Quat& unitQuat3)
	{
		return Quat::Slerp((2.0f * t) * (1.0f - t), Quat::Slerp(t, unitQuat0, unitQuat3), Quat::Slerp(t, unitQuat1, unitQuat2));
	}

	// Construct an identity quaternion
	static DESIRE_FORCE_INLINE Quat Identity()
	{
		return Quat(0.0f, 0.0f, 0.0f, 1.0f);
	}

	// Construct a quaternion to rotate around a unit-length 3-D vector
	static DESIRE_FORCE_INLINE Quat CreateRotation(float radians, const Vector3& unitVec)
	{
#if defined(DESIRE_USE_SSE)
		__m128 s, c;
		sincosf4(_mm_set1_ps(radians * 0.5f), &s, &c);
		return SIMD::Blend(SIMD::Mul(unitVec, s), c, SIMD::MaskW());
#else
		const float angle = radians * 0.5f;
		const float s = std::sin(angle);
		const float c = std::cos(angle);
		return Quat(unitVec * s, c);
#endif
	}

	// Construct a quaternion to rotate around the x axis
	static DESIRE_FORCE_INLINE Quat CreateRotationX(float radians)
	{
#if defined(DESIRE_USE_SSE)
		__m128 s, c;
		sincosf4(_mm_set1_ps(radians * 0.5f), &s, &c);
		const __m128 res = SIMD::Blend(_mm_setzero_ps(), s, SIMD::MaskX());
		return SIMD::Blend(res, c, SIMD::MaskW());
#else
		const float angle = radians * 0.5f;
		const float s = std::sin(angle);
		const float c = std::cos(angle);
		return Quat(s, 0.0f, 0.0f, c);
#endif
	}

	// Construct a quaternion to rotate around the y axis
	static DESIRE_FORCE_INLINE Quat CreateRotationY(float radians)
	{
#if defined(DESIRE_USE_SSE)
		__m128 s, c;
		sincosf4(_mm_set1_ps(radians * 0.5f), &s, &c);
		const __m128 res = SIMD::Blend(_mm_setzero_ps(), s, SIMD::MaskY());
		return SIMD::Blend(res, c, SIMD::MaskW());
#else
		const float angle = radians * 0.5f;
		const float s = std::sin(angle);
		const float c = std::cos(angle);
		return Quat(0.0f, s, 0.0f, c);
#endif
	}

	// Construct a quaternion to rotate around the z axis
	static DESIRE_FORCE_INLINE Quat CreateRotationZ(float radians)
	{
#if defined(DESIRE_USE_SSE)
		__m128 s, c;
		sincosf4(_mm_set1_ps(radians * 0.5f), &s, &c);
		const __m128 res = SIMD::Blend(_mm_setzero_ps(), s, SIMD::MaskZ());
		return SIMD::Blend(res, c, SIMD::MaskW());
#else
		const float angle = radians * 0.5f;
		const float s = std::sin(angle);
		const float c = std::cos(angle);
		return Quat(0.0f, 0.0f, s, c);
#endif
	}

	// Construct a quaternion to rotate using Euler angles for each axis
	static DESIRE_FORCE_INLINE Quat CreateRotationFromEulerAngles(const Vector3& radiansXYZ)
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
	// NOTE: The result is unpredictable if unitVec0 and unitVec1 point in opposite directions.
	static DESIRE_FORCE_INLINE Quat CreateRotationFromTo(const Vector3& unitVecFrom, const Vector3& unitVecTo)
	{
#if defined(DESIRE_USE_SSE)
		const __m128 cosAngle = SIMD::Dot3(unitVecFrom, unitVecTo);
		const __m128 cosAngleX2Plus2 = vec_madd(cosAngle, _mm_set1_ps(2.0f), _mm_set1_ps(2.0f));
		const __m128 recipCosHalfAngleX2 = _mm_rsqrt_ps(cosAngleX2Plus2);
		const __m128 cosHalfAngleX2 = SIMD::Mul(recipCosHalfAngleX2, cosAngleX2Plus2);
		const Vector3 crossVec = unitVecFrom.Cross(unitVecTo);
		const __m128 res = SIMD::Mul(crossVec, recipCosHalfAngleX2);
		alignas(16) const uint32_t select_w[4] = { 0, 0, 0, 0xffffffff };
		const __m128 mask_w = _mm_load_ps((float*)select_w);
		return SIMD::Blend(res, SIMD::Mul(cosHalfAngleX2, 0.5f), mask_w);
#else
		const float cosHalfAngleX2 = sqrtf((2.0f * (1.0f + unitVecFrom.Dot(unitVecTo))));
		const float recipCosHalfAngleX2 = (1.0f / cosHalfAngleX2);
		return Quat((unitVecFrom.Cross(unitVecTo) * recipCosHalfAngleX2), (cosHalfAngleX2 * 0.5f));
#endif
	}

private:
	vec_float4_t mVec128;
};

// Multiply a quaternion by a scalar
DESIRE_FORCE_INLINE Quat operator *(float scalar, const Quat& quat)
{
	return quat * scalar;
}
