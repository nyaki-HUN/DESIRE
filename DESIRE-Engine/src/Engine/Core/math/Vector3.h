#pragma once

#include "Engine/Core/math/vectormath/vectormath.h"

class Vector3
{
public:
	inline Vector3()
	{
		// No initialization
	}

	inline Vector3(const Vector3& vec)
		: mVec128(vec.mVec128)
	{

	}
	
	inline Vector3(vec_float3_t vf)
		: mVec128(vf)
	{

	}

	inline Vector3(float x, float y, float z)
		: mVec128(SIMD::Construct(x, y, z))
	{

	}

	explicit inline Vector3(float scalar)
	{
		SIMD::Construct(mVec128, scalar);
	}

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

	// Set element
	inline void SetX(float x)		{ SIMD::SetX(mVec128, x); }
	inline void SetY(float y)		{ SIMD::SetY(mVec128, y); }
	inline void SetZ(float z)		{ SIMD::SetZ(mVec128, z); }

	// Get element
	inline float GetX() const		{ return SIMD::GetX(mVec128); }
	inline float GetY() const		{ return SIMD::GetY(mVec128); }
	inline float GetZ() const		{ return SIMD::GetZ(mVec128); }

	// Operator overloads
	inline operator vec_float3_t() const
	{
		return mVec128;
	}

	inline Vector3& operator =(const Vector3& vec)
	{
		mVec128 = vec.mVec128;
		return *this;
	}

	inline Vector3 operator -() const						{ return SIMD::Negate(mVec128); }
	inline Vector3 operator +(const Vector3& vec) const		{ return SIMD::Add(mVec128, vec.mVec128); }
	inline Vector3 operator -(const Vector3& vec) const		{ return SIMD::Sub(mVec128, vec.mVec128); }
	inline Vector3 operator *(const Vector3& vec) const		{ return SIMD::Mul(mVec128, vec.mVec128); }
	inline Vector3 operator *(float scalar) const			{ return SIMD::Mul(mVec128, scalar); }
	inline Vector3 operator /(const Vector3& vec) const		{ return SIMD::Div(mVec128, vec.mVec128); }
	inline Vector3 operator /(float scalar) const			{ return SIMD::Mul(mVec128, 1.0f / scalar); }

	inline Vector3& operator +=(const Vector3& vec)			{ *this = *this + vec;		return *this; }
	inline Vector3& operator -=(const Vector3& vec)			{ *this = *this - vec;		return *this; }
	inline Vector3& operator *=(const Vector3& vec)			{ *this = *this * vec;		return *this; }
	inline Vector3& operator *=(float scalar)				{ *this = *this * scalar;	return *this; }
	inline Vector3& operator /=(const Vector3& vec)			{ *this = *this / vec;		return *this; }
	inline Vector3& operator /=(float scalar)				{ *this = *this / scalar;	return *this; }

	inline bool operator >(const Vector3& vec) const		{ return SIMD::OpCmpGT(mVec128, vec); }
	inline bool operator <(const Vector3& vec) const		{ return SIMD::OpCmpLT(mVec128, vec); }
	inline bool operator >=(const Vector3& vec) const		{ return SIMD::OpCmpGE(mVec128, vec); }
	inline bool operator <=(const Vector3& vec) const		{ return SIMD::OpCmpLE(mVec128, vec); }

	// Maximum element of a 3-D vector
	inline float GetMaxElem() const
	{
		return SIMD::GetX(SIMD::MaxElem3(mVec128));
	}

	// Minimum element of a 3-D vector
	inline float GetMinElem() const
	{
		return SIMD::GetX(SIMD::MinElem3(mVec128));
	}

	// Compute the dot product of two 3-D vectors
	inline float Dot(const Vector3& vec) const
	{
		return SIMD::GetX(SIMD::Dot3(mVec128, vec));
	}

	// Compute cross product of two 3-D vectors
	inline Vector3 Cross(const Vector3& vec) const
	{
		return SIMD::Cross(mVec128, vec);
	}

	// Compute the square of the length of a 3-D vector
	inline float LengthSqr() const
	{
		return Dot(*this);
	}

	// Compute the length of a 3-D vector
	inline float Length() const
	{
		return std::sqrt(LengthSqr());
	}

	// Normalize a 3-D vector
	// NOTE: The result is unpredictable when all elements of vec are at or near zero
	inline void Normalize()
	{
		mVec128 = SIMD::Mul(mVec128, newtonrapson_rsqrt4(SIMD::Dot3(mVec128, mVec128)));
	}

	// Get normalized 3-D vector
	// NOTE: The result is unpredictable when all elements of vec are at or near zero
	inline Vector3 Normalized() const
	{
		return SIMD::Mul(mVec128, newtonrapson_rsqrt4(SIMD::Dot3(mVec128, mVec128)));
	}

	// Compute the absolute value of a 3-D vector per element
	inline Vector3 AbsPerElem() const
	{
		return SIMD::AbsPerElem(mVec128);
	}

	// Maximum of two 3-D vectors per element
	static inline Vector3 MaxPerElem(const Vector3& vec0, const Vector3& vec1)
	{
		return SIMD::MaxPerElem(vec0, vec1);
	}

	// Minimum of two 3-D vectors per element
	static inline Vector3 MinPerElem(const Vector3& vec0, const Vector3& vec1)
	{
		return SIMD::MinPerElem(vec0, vec1);
	}

	// Spherical linear interpolation between two 3-D vectors
	// NOTE: 
	// The result is unpredictable if the vectors point in opposite directions.
	// Does not clamp t between 0 and 1.
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

	// Construct 3-D vector
	static inline Vector3 Zero()		{ return Vector3(0.0f); }
	static inline Vector3 AxisX()		{ return Vector3(1.0f, 0.0f, 0.0f); }
	static inline Vector3 AxisY()		{ return Vector3(0.0f, 1.0f, 0.0f); }
	static inline Vector3 AxisZ()		{ return Vector3(0.0f, 0.0f, 1.0f); }

private:
	vec_float3_t mVec128;
};

// Multiply a 3-D vector by a scalar
inline Vector3 operator *(float scalar, const Vector3& vec)
{
	return vec * scalar;
}
