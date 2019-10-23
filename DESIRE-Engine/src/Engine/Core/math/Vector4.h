#pragma once

#include "Engine/Core/math/Vector3.h"

class Vector4
{
public:
	inline Vector4()																			{}
	inline Vector4(const Vector4& vec)					: mVec128(vec.mVec128)					{}
	inline Vector4(vec_float4_t vf)						: mVec128(vf)							{}
	inline Vector4(float x, float y, float z, float w)	: mVec128(SIMD::Construct(x, y, z, w))	{}
	explicit inline Vector4(float scalar)														{ SIMD::Construct(mVec128, scalar); }
	inline Vector4(const Vector3& xyz, float w)			: mVec128(xyz)							{ SetW(w); }
	explicit inline Vector4(const Vector3& vec)			: Vector4(vec, 0.0f)					{}

	// Load x, y, z, and w elements from the first four elements of a float array
	inline void LoadXYZW(const float *fptr)					{ SIMD::LoadXYZW(mVec128, fptr); }

	// Store x, y, z, and w elements in the first four elements of a float array
	inline void StoreXYZW(float *fptr) const				{ SIMD::StoreXYZW(mVec128, fptr); }

	inline void SetXYZ(const Vector3& vec)					{ mVec128 = SIMD::Blend_W(vec, *this); }
	inline Vector3 GetXYZ() const							{ return mVec128; }

	inline void SetX(float x)								{ SIMD::SetX(mVec128, x); }
	inline void SetY(float y)								{ SIMD::SetY(mVec128, y); }
	inline void SetZ(float z)								{ SIMD::SetZ(mVec128, z); }
	inline void SetW(float w)								{ SIMD::SetW(mVec128, w); }

	inline float GetX() const								{ return SIMD::GetX(mVec128); }
	inline float GetY() const								{ return SIMD::GetY(mVec128); }
	inline float GetZ() const								{ return SIMD::GetZ(mVec128); }
	inline float GetW() const								{ return SIMD::GetW(mVec128); }

	inline operator vec_float4_t() const					{ return mVec128; }

	inline Vector4& operator =(const Vector4& vec)			{ mVec128 = vec.mVec128; return *this; }

	inline Vector4 operator -() const						{ return SIMD::Negate(*this); }
	inline Vector4 operator +(const Vector4& vec) const		{ return SIMD::Add(*this, vec); }
	inline Vector4 operator -(const Vector4& vec) const		{ return SIMD::Sub(*this, vec); }
	inline Vector4 operator *(const Vector4& vec) const		{ return SIMD::Mul(*this, vec); }
	inline Vector4 operator *(float scalar) const			{ return SIMD::Mul(*this, scalar); }
	inline Vector4 operator /(const Vector4& vec) const		{ return SIMD::Div(*this, vec); }
	inline Vector4 operator /(float scalar) const			{ return SIMD::Mul(*this, 1.0f / scalar); }

	inline Vector4& operator +=(const Vector4& vec)			{ *this = *this + vec;		return *this; }
	inline Vector4& operator -=(const Vector4& vec)			{ *this = *this - vec;		return *this; }
	inline Vector4& operator *=(const Vector4& vec)			{ *this = *this * vec;		return *this; }
	inline Vector4& operator *=(float scalar)				{ *this = *this * scalar;	return *this; }
	inline Vector4& operator /=(const Vector4& vec)			{ *this = *this / vec;		return *this; }
	inline Vector4& operator /=(float scalar)				{ *this = *this / scalar;	return *this; }

	inline float Dot(const Vector4& vec) const				{ return SIMD::GetX(SIMD::Dot4(*this, vec)); }

	inline float LengthSqr() const							{ return Dot(*this); }
	inline float Length() const								{ return std::sqrt(LengthSqr()); }

	inline void Normalize()									{ mVec128 = Normalized(); }
	inline Vector4 Normalized() const						{ return SIMD::Mul(*this, newtonrapson_rsqrt4(SIMD::Dot4(*this, *this))); }

	inline Vector4 AbsPerElem() const						{ return SIMD::AbsPerElem(*this); }

	inline float GetMaxElem() const							{ return SIMD::GetX(SIMD::MaxElem4(*this)); }
	inline float GetMinElem() const							{ return SIMD::GetX(SIMD::MinElem4(*this)); }

	static inline Vector4 MaxPerElem(const Vector4& a, const Vector4& b)	{ return SIMD::MaxPerElem(a, b); }
	static inline Vector4 MinPerElem(const Vector4& a, const Vector4& b)	{ return SIMD::MinPerElem(a, b); }

	// Spherical linear interpolation
	// NOTE: The result is unpredictable if the vectors point in opposite directions. Doesn't clamp t between 0 and 1.
	static inline Vector4 Slerp(float t, const Vector4& unitVec0, const Vector4& unitVec1)
	{
		float scale0, scale1;
		const float cosAngle = unitVec0.Dot(unitVec1);
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
		return unitVec0 * scale0 + unitVec1 * scale1;
	}

	static inline Vector4 Zero()		{ return Vector4(0.0f); }
	static inline Vector4 AxisX()		{ return Vector4(1.0f, 0.0f, 0.0f, 0.0f); }
	static inline Vector4 AxisY()		{ return Vector4(0.0f, 1.0f, 0.0f, 0.0f); }
	static inline Vector4 AxisZ()		{ return Vector4(0.0f, 0.0f, 1.0f, 0.0f); }
	static inline Vector4 AxisW()		{ return Vector4(0.0f, 0.0f, 0.0f, 1.0f); }

private:
	vec_float4_t mVec128;
};

inline Vector4 operator *(float scalar, const Vector4& vec)
{
	return vec * scalar;
}
