#pragma once

#include "Engine/Core/math/SIMD.h"

#include <cfloat>	// for FLT_MAX

class Vector3
{
public:
	inline Vector3()																{}
	inline Vector3(simd128_t vec)				: vec128(vec)						{}
	inline Vector3(float x, float y, float z)	: vec128(SIMD::Construct(x, y, z))	{}
	explicit inline Vector3(float scalar)		: vec128(SIMD::Construct(scalar))	{}

	// Load x, y, and z elements from the first three elements of a float array
	inline void LoadXYZ(const float *fptr)
	{
		*this = Vector3(fptr[0], fptr[1], fptr[2]);
	}

	// Store x, y, and z elements in the first three elements of a float array
	inline void StoreXYZ(float *fptr) const
	{
		fptr[0] = GetX();
		fptr[1] = GetY();
		fptr[2] = GetZ();
	}

	inline void SetX(float x)								{ vec128 = SIMD::SetX(*this, x); }
	inline void SetY(float y)								{ vec128 = SIMD::SetY(*this, y); }
	inline void SetZ(float z)								{ vec128 = SIMD::SetZ(*this, z); }

	inline float GetX() const								{ return SIMD::GetX(*this); }
	inline float GetY() const								{ return SIMD::GetY(*this); }
	inline float GetZ() const								{ return SIMD::GetZ(*this); }

	inline operator simd128_t() const						{ return vec128; }

	inline Vector3& operator =(const Vector3& vec)			{ vec128 = vec; return *this; }

	inline Vector3 operator -() const						{ return SIMD::Negate(*this); }
	inline Vector3 operator +(const Vector3& vec) const		{ return SIMD::Add(*this, vec); }
	inline Vector3 operator -(const Vector3& vec) const		{ return SIMD::Sub(*this, vec); }
	inline Vector3 operator *(const Vector3& vec) const		{ return SIMD::Mul(*this, vec); }
	inline Vector3 operator *(float scalar) const			{ return SIMD::Mul(*this, scalar); }
	inline Vector3 operator /(const Vector3& vec) const		{ return SIMD::Div(*this, vec); }
	inline Vector3 operator /(float scalar) const			{ return SIMD::Mul(*this, 1.0f / scalar); }

	inline Vector3& operator +=(const Vector3& vec)			{ *this = *this + vec;		return *this; }
	inline Vector3& operator -=(const Vector3& vec)			{ *this = *this - vec;		return *this; }
	inline Vector3& operator *=(const Vector3& vec)			{ *this = *this * vec;		return *this; }
	inline Vector3& operator *=(float scalar)				{ *this = *this * scalar;	return *this; }
	inline Vector3& operator /=(const Vector3& vec)			{ *this = *this / vec;		return *this; }
	inline Vector3& operator /=(float scalar)				{ *this = *this / scalar;	return *this; }

	inline bool operator >(const Vector3& vec) const		{ return SIMD::OpCmp3_GT(*this, vec); }
	inline bool operator <(const Vector3& vec) const		{ return SIMD::OpCmp3_LT(*this, vec); }
	inline bool operator >=(const Vector3& vec) const		{ return SIMD::OpCmp3_GE(*this, vec); }
	inline bool operator <=(const Vector3& vec) const		{ return SIMD::OpCmp3_LE(*this, vec); }

	inline float Dot(const Vector3& vec) const				{ return SIMD::GetX(SIMD::Dot3(*this, vec)); }
	inline Vector3 Cross(const Vector3& vec) const			{ return SIMD::Cross(*this, vec); }

	inline float LengthSqr() const							{ return Dot(*this); }
	inline float Length() const								{ return std::sqrt(LengthSqr()); }

	inline void Normalize()									{ *this = Normalized(); }
	inline Vector3 Normalized() const						{ return SIMD::Mul(*this, SIMD::InvSqrt(SIMD::Dot3(*this, *this))); }

	inline Vector3 AbsPerElem() const						{ return SIMD::AbsPerElem(*this); }

	inline float GetMaxElem() const							{ return SIMD::GetX(SIMD::MaxElem(SIMD::SetW(*this, -FLT_MAX))); }
	inline float GetMinElem() const							{ return SIMD::GetX(SIMD::MinElem(SIMD::SetW(*this, FLT_MAX))); }

	static inline Vector3 MaxPerElem(const Vector3& a, const Vector3& b)	{ return SIMD::MaxPerElem(a, b); }
	static inline Vector3 MinPerElem(const Vector3& a, const Vector3& b)	{ return SIMD::MinPerElem(a, b); }

	// Spherical linear interpolation
	// NOTE: The result is unpredictable if the vectors point in opposite directions. Doesn't clamp t between 0 and 1.
	static inline Vector3 Slerp(float t, const Vector3& unitVec0, const Vector3& unitVec1)
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

	static inline Vector3 Zero()		{ return Vector3(0.0f); }
	static inline Vector3 One()			{ return Vector3(1.0f); }
	static inline Vector3 AxisX()		{ return Vector3(1.0f, 0.0f, 0.0f); }
	static inline Vector3 AxisY()		{ return Vector3(0.0f, 1.0f, 0.0f); }
	static inline Vector3 AxisZ()		{ return Vector3(0.0f, 0.0f, 1.0f); }

private:
	simd128_t vec128;
};

inline Vector3 operator *(float scalar, const Vector3& vec)
{
	return vec * scalar;
}
