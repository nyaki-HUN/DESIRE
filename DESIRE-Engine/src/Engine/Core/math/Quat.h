#pragma once

#include "Engine/Core/math/Vector3.h"

class Matrix3;

class Quat
{
public:
	inline Quat()																			{}
	inline Quat(const Quat& vec)					: mVec128(vec.mVec128)					{}
	inline Quat(vec_float4_t vf4)					: mVec128(vf4)							{}
	inline Quat(float x, float y, float z, float w) : mVec128(SIMD::Construct(x, y, z, w))	{}
	explicit inline Quat(const Matrix3& rotMat);

	// Load x, y, z, and w elements from the first four elements of a float array
	inline void LoadXYZW(const float *fptr)					{ SIMD::LoadXYZW(mVec128, fptr); }

	// Store x, y, z, and w elements in the first four elements of a float array
	inline void StoreXYZW(float *fptr) const				{ SIMD::StoreXYZW(mVec128, fptr); }

	inline void SetX(float x)								{ SIMD::SetX(mVec128, x); }
	inline void SetY(float y)								{ SIMD::SetY(mVec128, y); }
	inline void SetZ(float z)								{ SIMD::SetZ(mVec128, z); }
	inline void SetW(float w)								{ SIMD::SetW(mVec128, w); }

	inline float GetX() const								{ return SIMD::GetX(mVec128); }
	inline float GetY() const								{ return SIMD::GetY(mVec128); }
	inline float GetZ() const								{ return SIMD::GetZ(mVec128); }
	inline float GetW() const								{ return SIMD::GetW(mVec128); }

	inline operator vec_float4_t() const					{ return mVec128; }

	inline Quat& operator =(const Quat& quat)				{ mVec128 = quat; return *this; }

	inline Quat operator -() const							{ return SIMD::Negate(*this); }
	inline Quat operator +(const Quat& quat) const			{ return SIMD::Add(*this, quat); }
	inline Quat operator -(const Quat& quat) const			{ return SIMD::Sub(*this, quat); }
	inline Quat operator *(const Quat& quat) const;

	inline Quat& operator +=(const Quat& quat)				{ *this = *this + quat; return *this; }
	inline Quat& operator -=(const Quat& quat)				{ *this = *this - quat; return *this; }
	inline Quat& operator *=(const Quat& quat)				{ *this = *this * quat; return *this; }

	inline float Dot(const Quat& quat) const				{ return SIMD::GetX(SIMD::Dot4(*this, quat)); }

	inline float Norm() const								{ return Dot(*this); }
	inline float Length() const								{ return std::sqrt(Norm()); }

	inline Quat Conjugate() const;

	// Compute the Euler angle representation of the rotation in radians
	inline Vector3 EulerAngles() const
	{
		const vec_float4_t vecSq2 = SIMD::Mul(SIMD::Mul(*this, *this), 2.0f);
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

	inline Vector3 RotateVec(const Vector3& vec) const;

	inline void Normalize()									{ mVec128 = Normalized(); }
	inline Quat Normalized() const							{ return SIMD::Mul(*this, newtonrapson_rsqrt4(SIMD::Dot4(*this, *this))); }

	// Spherical linear interpolation
	// NOTE: The result is unpredictable if the vectors point in opposite directions. Doesn't clamp t between 0 and 1.
	static inline Quat Slerp(float t, const Quat& unitQuat0, const Quat& unitQuat1);

	// Spherical quadrangle interpolation
	static inline Quat Squad(float t, const Quat& unitQuat0, const Quat& unitQuat1, const Quat& unitQuat2, const Quat& unitQuat3)
	{
		return Quat::Slerp(
			(2.0f * t) * (1.0f - t),
			Quat::Slerp(t, unitQuat0, unitQuat3),
			Quat::Slerp(t, unitQuat1, unitQuat2)
		);
	}

	static inline Quat Identity()							{ return Quat(0.0f, 0.0f, 0.0f, 1.0f); }

	static inline Quat CreateRotationX(float radians);
	static inline Quat CreateRotationY(float radians);
	static inline Quat CreateRotationZ(float radians);

	// Construct a quaternion to rotate around a unit-length 3-D vector
	static inline Quat CreateRotation(float radians, const Vector3& unitVec);

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
	const __m128 sign_wzyx = SIMD::Construct(0.0f, -0.0f, 0.0f, -0.0f);
	const __m128 sign_zwxy = SIMD::Construct(0.0f, 0.0f, -0.0f, -0.0f);
	const __m128 sign_yxwz = SIMD::Construct(-0.0f, 0.0f, 0.0f, -0.0f);

	const __m128 w0_xyzw1 = SIMD::Mul(SIMD::Swizzle_WWWW(*this), quat);
	const __m128 x0_wzyx1 = SIMD::Mul(SIMD::Swizzle_XXXX(*this), SIMD::Swizzle_WZYX(quat));
	const __m128 y0_zwxy1 = SIMD::Mul(SIMD::Swizzle_YYYY(*this), SIMD::Swizzle_ZWXY(quat));
	const __m128 z0_yxwz1 = SIMD::Mul(SIMD::Swizzle_ZZZZ(*this), SIMD::Swizzle_YXWZ(quat));

	__m128 result = SIMD::Add(w0_xyzw1, _mm_xor_ps(x0_wzyx1, sign_wzyx));
	result = SIMD::Add(result, _mm_xor_ps(y0_zwxy1, sign_zwxy));
	return SIMD::Add(result, _mm_xor_ps(z0_yxwz1, sign_yxwz));
#elif defined(__ARM_NEON__)
	const float32x4_t sign_wzyx = SIMD::Construct(1.0f, -1.0f, 1.0f, -1.0f);
	const float32x4_t sign_zwxy = SIMD::Construct(1.0f, 1.0f, -1.0f, -1.0f);
	const float32x4_t sign_yxwz = SIMD::Construct(-1.0f, 1.0f, 1.0f, -1.0f);

	const float32x2_t xy0 = vget_low_f32(*this);
	const float32x2_t zw0 = vget_high_f32(*this);
	const float32x4_t yxwz1 = SIMD::Swizzle_YXWZ(quat);

	const float32x4_t w0_xyzw1 = vmulq_lane_f32(quat, zw0, 1);
	const float32x4_t x0_wzyx1 = vmulq_lane_f32(SIMD::Swizzle_ZWXY(yxwz1), xy0, 0);
	const float32x4_t y0_zwxy1 = vmulq_lane_f32(SIMD::Swizzle_ZWXY(quat), xy0, 1);
	const float32x4_t z0_yxwz1 = vmulq_lane_f32(yxwz1, zw0, 0);

	float32x4_t result = SIMD::MulAdd(x0_wzyx1, sign_wzyx, w0_xyzw1);
	result = SIMD::MulAdd(y0_zwxy1, sign_zwxy, result);
	return SIMD::MulAdd(z0_yxwz1, sign_yxwz, result);
#else
	return Quat(
		GetW() * quat.GetX()  +  GetX() * quat.GetW()  +  GetY() * quat.GetZ()  -  GetZ() * quat.GetY(),
		GetW() * quat.GetY()  -  GetX() * quat.GetZ()  +  GetY() * quat.GetW()  +  GetZ() * quat.GetX(),
		GetW() * quat.GetZ()  +  GetX() * quat.GetY()  -  GetY() * quat.GetX()  +  GetZ() * quat.GetW(),
		GetW() * quat.GetW()  -  GetX() * quat.GetX()  -  GetY() * quat.GetY()  -  GetZ() * quat.GetZ()
	);
#endif
}

// Compute the conjugate of a quaternion
inline Quat Quat::Conjugate() const
{
#if defined(DESIRE_USE_SSE)
	const __m128 mask = SIMD::Construct(-0.0f, -0.0f, -0.0f, 0.0f);
	return _mm_xor_ps(*this, mask);
#elif defined(__ARM_NEON__)
	Quat conjugate = SIMD::Negate(*this);
	conjugate.SetW(GetW());
	return conjugate;
#else
	return Quat(-GetX(), -GetY(), -GetZ(), GetW());
#endif
}

// Rotate a 3-D vector with this unit-length quaternion
inline Vector3 Quat::RotateVec(const Vector3& vec) const
{
#if defined(DESIRE_USE_SSE)
	const __m128 tmp0 = SIMD::Swizzle_YZXW(*this);
	__m128 tmp1 = SIMD::Swizzle_ZXYW(vec);
	const __m128 tmp2 = SIMD::Swizzle_ZXYW(*this);
	__m128 tmp3 = SIMD::Swizzle_YZXW(vec);
	const __m128 wwww = SIMD::Swizzle_WWWW(*this);
	__m128 qv = SIMD::Mul(wwww, vec);
	qv = SIMD::MulAdd(tmp0, tmp1, qv);
	qv = SIMD::MulSub(tmp2, tmp3, qv);
	const __m128 product = SIMD::Mul(*this, vec);
	__m128 qw = SIMD::MulAdd(_mm_ror_ps(*this, 1), _mm_ror_ps(vec, 1), product);
	qw = SIMD::Add(_mm_ror_ps(product, 2), qw);
	tmp1 = SIMD::Swizzle_ZXYW(qv);
	tmp3 = SIMD::Swizzle_YZXW(qv);
	__m128 res = SIMD::Mul(SIMD::Swizzle_XXXX(qw), *this);
	res = SIMD::MulAdd(wwww, qv, res);
	res = SIMD::MulAdd(tmp0, tmp1, res);
	res = SIMD::MulSub(tmp2, tmp3, res);
	return res;
#else
	const float tmpX = GetW() * vec.GetX()  +  GetY() * vec.GetZ()  -  GetZ() * vec.GetY();
	const float tmpY = GetW() * vec.GetY()  +  GetZ() * vec.GetX()  -  GetX() * vec.GetZ();
	const float tmpZ = GetW() * vec.GetZ()  +  GetX() * vec.GetY()  -  GetY() * vec.GetX();
	const float tmpW = GetX() * vec.GetX()  +  GetY() * vec.GetY()  +  GetZ() * vec.GetZ();
	return Vector3(
		tmpW * GetX()  +  tmpX * GetW()  -  tmpY * GetZ()  +  tmpZ * GetY(),
		tmpW * GetY()  +  tmpY * GetW()  -  tmpZ * GetX()  +  tmpX * GetZ(),
		tmpW * GetZ()  +  tmpZ * GetW()  -  tmpX * GetY()  +  tmpY * GetX()
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
	selectMask = _mm_cmpgt_ps(_mm_set1_ps(0.999f), cosAngle);
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
		start = -unitQuat0;
	}
	else
	{
		start = unitQuat0;
	}

	if(cosAngle < 0.999f)
	{
		const float angle = std::acos(cosAngle);
		const float recipSinAngle = 1.0f / std::sin(angle);
		scale0 = std::sin((1.0f - t) * angle) * recipSinAngle;
		scale1 = std::sin(t * angle) * recipSinAngle;
	}
	else
	{
		scale0 = 1.0f - t;
		scale1 = t;
	}

	return start * scale0 + unitQuat1 * scale1;
#endif
}

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

// Construct a quaternion to rotate using Euler angles for each axis
inline Quat Quat::CreateRotationFromEulerAngles(const Vector3& radiansXYZ)
{
	const Vector3 halfAngle = radiansXYZ * 0.5f;
	const float cX = std::cos(halfAngle.GetX());
	const float sX = std::sin(halfAngle.GetX());
	const float cY = std::cos(halfAngle.GetY());
	const float sY = std::sin(halfAngle.GetY());
	const float cZ = std::cos(halfAngle.GetZ());
	const float sZ = std::sin(halfAngle.GetZ());

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
