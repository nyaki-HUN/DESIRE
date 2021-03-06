#pragma once

#include "Engine/Core/Math/Vector3.h"

class Matrix3;

class Quat
{
public:
	inline Quat() = default;
	inline Quat(simd128_t vec)						: m_vec128(vec)							{}
	inline Quat(float x, float y, float z, float w) : m_vec128(SIMD::Construct(x, y, z, w))	{}
	explicit inline Quat(const Matrix3& rotMat);

	// Load x, y, z, and w elements from the first four elements of a float array
	inline void LoadXYZW(const float* pValues)				{ m_vec128 = SIMD::LoadXYZW(pValues); }

	// Store x, y, z, and w elements in the first four elements of a float array
	inline void StoreXYZW(float* pValues) const				{ SIMD::StoreXYZW(*this, pValues); }

	inline void SetX(float x)								{ m_vec128 = SIMD::SetX(*this, x); }
	inline void SetY(float y)								{ m_vec128 = SIMD::SetY(*this, y); }
	inline void SetZ(float z)								{ m_vec128 = SIMD::SetZ(*this, z); }
	inline void SetW(float w)								{ m_vec128 = SIMD::SetW(*this, w); }

	inline float GetX() const								{ return SIMD::GetX(*this); }
	inline float GetY() const								{ return SIMD::GetY(*this); }
	inline float GetZ() const								{ return SIMD::GetZ(*this); }
	inline float GetW() const								{ return SIMD::GetW(*this); }

	inline operator simd128_t() const						{ return m_vec128; }

	inline Quat& operator =(const Quat& quat)				{ m_vec128 = quat; return *this; }

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
	inline Vector3 EulerAngles() const;

	// Rotate a 3-D vector with this unit-length quaternion
	inline Vector3 RotateVec(const Vector3& vec) const
	{
		const Vector3 t = SIMD::Mul(SIMD::Cross(*this, vec), 2.0f);
		return SIMD::MulAdd(SIMD::Swizzle_WWWW(*this), t, SIMD::Add(vec, SIMD::Cross(*this, t)));
	}

	inline void Normalize()									{ *this = Normalized(); }
	inline Quat Normalized() const							{ return SIMD::Mul(*this, SIMD::InvSqrt(SIMD::Dot4(*this, *this))); }

	static inline Quat Identity()							{ return Quat(0.0f, 0.0f, 0.0f, 1.0f); }

	static inline Quat CreateRotationX(float radians);
	static inline Quat CreateRotationY(float radians);
	static inline Quat CreateRotationZ(float radians);

	// Construct a quaternion to rotate around a unit-length 3-D vector
	static inline Quat CreateRotation(float radians, const Vector3& unitVec);

	// Construct a quaternion to rotate using Euler angles for each axis
	static inline Quat CreateRotationFromEulerAngles(const Vector3& radiansXYZ);

private:
	simd128_t m_vec128;
};

// --------------------------------------------------------------------------------------------------------------------

// Multiply two quaternions
inline Quat Quat::operator *(const Quat& quat) const
{
#if DESIRE_USE_SSE
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
#elif DESIRE_USE_NEON
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
#if DESIRE_USE_SSE
	const __m128 mask = SIMD::Construct(-0.0f, -0.0f, -0.0f, 0.0f);
	return _mm_xor_ps(*this, mask);
#elif DESIRE_USE_NEON
	return SIMD::SetW(SIMD::Negate(*this), GetW());
#else
	return Quat(-GetX(), -GetY(), -GetZ(), GetW());
#endif
}

// Compute the Euler angle representation of the rotation in radians
inline Vector3 Quat::EulerAngles() const
{
	const simd128_t vecSq2 = SIMD::Mul(SIMD::Mul(*this, *this), 2.0f);
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

inline Quat Quat::CreateRotationX(float radians)
{
	const float halfAngle = radians * 0.5f;
	const float s = std::sin(halfAngle);
	const float c = std::cos(halfAngle);
	return Quat(s, 0.0f, 0.0f, c);
}

inline Quat Quat::CreateRotationY(float radians)
{
	const float halfAngle = radians * 0.5f;
	const float s = std::sin(halfAngle);
	const float c = std::cos(halfAngle);
	return Quat(0.0f, s, 0.0f, c);
}

inline Quat Quat::CreateRotationZ(float radians)
{
	const float halfAngle = radians * 0.5f;
	const float s = std::sin(halfAngle);
	const float c = std::cos(halfAngle);
	return Quat(0.0f, 0.0f, s, c);
}

// Construct a quaternion to rotate around a unit-length 3-D vector
inline Quat Quat::CreateRotation(float radians, const Vector3& unitVec)
{
	const float halfAngle = radians * 0.5f;
	const float s = std::sin(halfAngle);
	const float c = std::cos(halfAngle);
	return SIMD::SetW(SIMD::Mul(unitVec, s), c);
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
