#pragma once

#include "Engine/Core/Math/Vector3.h"

class Vector4
{
public:
	inline Vector4() = default;
	inline Vector4(simd128_t vec)						: m_vec128(vec)							{}
	inline Vector4(float x, float y, float z, float w)	: m_vec128(SIMD::Construct(x, y, z, w))	{}
	inline Vector4(const Vector3& xyz, float w)			: m_vec128(SIMD::SetW(xyz, w))			{}
	explicit inline Vector4(const Vector3& vec)			: Vector4(vec, 0.0f)					{}

	// Load x, y, z, and w elements from the first four elements of a float array
	inline void LoadXYZW(const float* pValues)				{ m_vec128 = SIMD::LoadXYZW(pValues); }

	// Store x, y, z, and w elements in the first four elements of a float array
	inline void StoreXYZW(float* pValues) const				{ SIMD::StoreXYZW(*this, pValues); }

	inline void SetXYZ(const Vector3& vec)					{ m_vec128 = SIMD::Blend_W(vec, *this); }
	inline Vector3 GetXYZ() const							{ return m_vec128; }

	inline void SetX(float x)								{ m_vec128 = SIMD::SetX(*this, x); }
	inline void SetY(float y)								{ m_vec128 = SIMD::SetY(*this, y); }
	inline void SetZ(float z)								{ m_vec128 = SIMD::SetZ(*this, z); }
	inline void SetW(float w)								{ m_vec128 = SIMD::SetW(*this, w); }

	inline float GetX() const								{ return SIMD::GetX(*this); }
	inline float GetY() const								{ return SIMD::GetY(*this); }
	inline float GetZ() const								{ return SIMD::GetZ(*this); }
	inline float GetW() const								{ return SIMD::GetW(*this); }

	inline Vector4 WithX(float x) const						{ return SIMD::SetX(*this, x); }
	inline Vector4 WithY(float y) const						{ return SIMD::SetY(*this, y); }
	inline Vector4 WithZ(float z) const						{ return SIMD::SetZ(*this, z); }
	inline Vector4 WithW(float w) const						{ return SIMD::SetW(*this, w); }

	inline operator simd128_t() const						{ return m_vec128; }

	inline Vector4& operator =(const Vector4& vec)			{ m_vec128 = vec; return *this; }

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

	inline bool operator >(const Vector4& vec) const		{ return SIMD::OpCmp_GT(*this, vec); }
	inline bool operator <(const Vector4& vec) const		{ return SIMD::OpCmp_LT(*this, vec); }
	inline bool operator >=(const Vector4& vec) const		{ return SIMD::OpCmp_GE(*this, vec); }
	inline bool operator <=(const Vector4& vec) const		{ return SIMD::OpCmp_LE(*this, vec); }

	inline float Dot(const Vector4& vec) const				{ return SIMD::GetX(SIMD::Dot4(*this, vec)); }

	inline float LengthSqr() const							{ return Dot(*this); }
	inline float Length() const								{ return std::sqrt(LengthSqr()); }

	inline void Normalize()									{ *this = Normalized(); }
	inline Vector4 Normalized() const						{ return SIMD::Mul(*this, SIMD::InvSqrt(SIMD::Dot4(*this, *this))); }

	inline Vector4 AbsPerElem() const						{ return SIMD::AbsPerElem(*this); }

	inline float GetMaxElem() const							{ return SIMD::GetX(SIMD::MaxElem(*this)); }
	inline float GetMinElem() const							{ return SIMD::GetX(SIMD::MinElem(*this)); }

	static inline Vector4 MaxPerElem(const Vector4& a, const Vector4& b)	{ return SIMD::MaxPerElem(a, b); }
	static inline Vector4 MinPerElem(const Vector4& a, const Vector4& b)	{ return SIMD::MinPerElem(a, b); }

	static inline Vector4 Zero()		{ return SIMD::ConstructZero(); }
	static inline Vector4 One()			{ return SIMD::Construct(1.0f); }
	static inline Vector4 AxisX()		{ return Vector4(1.0f, 0.0f, 0.0f, 0.0f); }
	static inline Vector4 AxisY()		{ return Vector4(0.0f, 1.0f, 0.0f, 0.0f); }
	static inline Vector4 AxisZ()		{ return Vector4(0.0f, 0.0f, 1.0f, 0.0f); }
	static inline Vector4 AxisW()		{ return Vector4(0.0f, 0.0f, 0.0f, 1.0f); }

private:
	simd128_t m_vec128;
};

inline Vector4 operator *(float scalar, const Vector4& vec)
{
	return vec * scalar;
}
