#pragma once

#include "Engine/Core/Math/SIMD.h"

class Vector3
{
public:
	inline Vector3() = default;
	inline Vector3(simd128_t vec)				: m_vec128(vec)							{}
	inline Vector3(float x, float y, float z)	: m_vec128(SIMD::Construct(x, y, z))	{}

	// Load x, y, and z elements from the first three elements of a float array
	inline void LoadXYZ(const float* pValues)				{ *this = Vector3(pValues[0], pValues[1], pValues[2]); }

	// Store x, y, and z elements in the first three elements of a float array
	inline void StoreXYZ(float* pValues) const
	{
		pValues[0] = GetX();
		pValues[1] = GetY();
		pValues[2] = GetZ();
	}

	inline void SetX(float x)								{ m_vec128 = SIMD::SetX(*this, x); }
	inline void SetY(float y)								{ m_vec128 = SIMD::SetY(*this, y); }
	inline void SetZ(float z)								{ m_vec128 = SIMD::SetZ(*this, z); }

	inline float GetX() const								{ return SIMD::GetX(*this); }
	inline float GetY() const								{ return SIMD::GetY(*this); }
	inline float GetZ() const								{ return SIMD::GetZ(*this); }

	inline Vector3 WithX(float x) const						{ return SIMD::SetX(*this, x); }
	inline Vector3 WithY(float y) const						{ return SIMD::SetY(*this, y); }
	inline Vector3 WithZ(float z) const						{ return SIMD::SetZ(*this, z); }

	inline operator simd128_t() const						{ return m_vec128; }

	inline Vector3& operator =(const Vector3& vec)			{ m_vec128 = vec; return *this; }

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

	static inline Vector3 Zero()		{ return SIMD::ConstructZero(); }
	static inline Vector3 One()			{ return SIMD::Construct(1.0f); }
	static inline Vector3 AxisX()		{ return Vector3(1.0f, 0.0f, 0.0f); }
	static inline Vector3 AxisY()		{ return Vector3(0.0f, 1.0f, 0.0f); }
	static inline Vector3 AxisZ()		{ return Vector3(0.0f, 0.0f, 1.0f); }

private:
	simd128_t m_vec128;
};

inline Vector3 operator *(float scalar, const Vector3& vec)
{
	return vec * scalar;
}
