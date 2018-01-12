#pragma once

class Vector4
{
public:
	DESIRE_FORCE_INLINE Vector4()
	{
		// No initialization
	}

	DESIRE_FORCE_INLINE Vector4(const Vector4& vec)
		: mVec128(vec.mVec128)
	{
	
	}
	
	DESIRE_FORCE_INLINE Vector4(vec_float4_t vf)
		: mVec128(vf) 
	{
	
	}

	DESIRE_FORCE_INLINE Vector4(float x, float y, float z, float w)
	{
		SIMD::Construct(mVec128, x, y, z, w);
	}

	explicit DESIRE_FORCE_INLINE Vector4(float scalar)
	{
		SIMD::Construct(mVec128, scalar);
	}

	// Construct a 4-D vector from a 3-D vector and a scalar
	DESIRE_FORCE_INLINE Vector4(const Vector3& xyz, float w)
	{
		mVec128 = xyz;
		SetW(w);
	}

	// Copy x, y, and z from a 3-D vector into a 4-D vector, and set w to 0
	explicit DESIRE_FORCE_INLINE Vector4(const Vector3& vec)
		: Vector4(vec, 0.0f)
	{

	}

	// Load x, y, z, and w elements from the first four elements of a float array
	DESIRE_FORCE_INLINE void LoadXYZW(const float *fptr)
	{
		SIMD::LoadXYZW(mVec128, fptr);
	}

	// Store x, y, z, and w elements of a 4-D vector in the first four elements of a float array
	DESIRE_FORCE_INLINE void StoreXYZW(float *fptr) const
	{
		SIMD::StoreXYZW(mVec128, fptr);
	}

	// Set the x, y, and z elements of a 4-D vector (does not change the w element)
	DESIRE_FORCE_INLINE void SetXYZ(const Vector3& vec)
	{
		SIMD::SetXYZ(mVec128, vec);
	}

	// Get the x, y, and z elements of a 4-D vector
	DESIRE_FORCE_INLINE Vector3 GetXYZ() const
	{
		return mVec128;
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

	DESIRE_FORCE_INLINE Vector4& operator =(const Vector4& vec)
	{
		mVec128 = vec.mVec128;
		return *this;
	}

	DESIRE_FORCE_INLINE Vector4 operator -() const						{ return SIMD::Negate(mVec128); }
	DESIRE_FORCE_INLINE Vector4 operator +(const Vector4& vec) const	{ return SIMD::Add(mVec128, vec.mVec128); }
	DESIRE_FORCE_INLINE Vector4 operator -(const Vector4& vec) const	{ return SIMD::Sub(mVec128, vec.mVec128); }
	DESIRE_FORCE_INLINE Vector4 operator *(float scalar) const			{ return SIMD::Mul(mVec128, scalar); }

	DESIRE_FORCE_INLINE Vector4 operator /(float scalar) const
	{
		return (*this * (1.0f / scalar));
	}

	DESIRE_FORCE_INLINE Vector4& operator +=(const Vector4& vec)
	{
		*this = *this + vec;
		return *this;
	}

	DESIRE_FORCE_INLINE Vector4& operator -=(const Vector4& vec)
	{
		*this = *this - vec;
		return *this;
	}

	DESIRE_FORCE_INLINE Vector4& operator *=(float scalar)
	{
		*this = *this * scalar;
		return *this;
	}

	DESIRE_FORCE_INLINE Vector4& operator /=(float scalar)
	{
		*this = *this / scalar;
		return *this;
	}

	// Maximum element of a 4-D vector
	DESIRE_FORCE_INLINE float GetMaxElem() const
	{
		return SIMD::GetX(SIMD::MaxElem4(mVec128));
	}

	// Minimum element of a 4-D vector
	DESIRE_FORCE_INLINE float GetMinElem() const
	{
		return SIMD::GetX(SIMD::MinElem4(mVec128));
	}

	// Compute the dot product of two 4-D vectors
	DESIRE_FORCE_INLINE float Dot(const Vector4& vec) const
	{
		return SIMD::GetX(SIMD::Dot4(mVec128, vec));
	}

	// Compute the square of the length of a 4-D vector
	DESIRE_FORCE_INLINE float LengthSqr() const
	{
		return Dot(*this);
	}

	// Compute the length of a 4-D vector
	DESIRE_FORCE_INLINE float Length() const
	{
		return std::sqrtf(LengthSqr());
	}

	// Normalize a 4-D vector
	// NOTE: The result is unpredictable when all elements of vec are at or near zero
	DESIRE_FORCE_INLINE void Normalize()
	{
		mVec128 = SIMD::Mul(mVec128, newtonrapson_rsqrt4(SIMD::Dot4(mVec128, mVec128)));
	}

	// Get normalized 4-D vector
	// NOTE: The result is unpredictable when all elements of vec are at or near zero
	DESIRE_FORCE_INLINE Vector4 Normalized() const
	{
		return SIMD::Mul(mVec128, newtonrapson_rsqrt4(SIMD::Dot4(mVec128, mVec128)));
	}

	// Multiply two 4-D vectors per element
	DESIRE_FORCE_INLINE Vector4 MulPerElem(const Vector4& vec) const
	{
		return SIMD::Mul(mVec128, vec);
	}

	// Divide two 4-D vectors per element
	DESIRE_FORCE_INLINE Vector4 DivPerElem(const Vector4& vec) const
	{
		return SIMD::Div(mVec128, vec);
	}

	// Compute the absolute value of a 4-D vector per element
	DESIRE_FORCE_INLINE Vector4 AbsPerElem() const
	{
		return SIMD::AbsPerElem(mVec128);
	}

	// Maximum of two 4-D vectors per element
	static DESIRE_FORCE_INLINE Vector4 MaxPerElem(const Vector4& vec0, const Vector4& vec1)
	{
		return SIMD::MaxPerElem(vec0, vec1);
	}

	// Minimum of two 4-D vectors per element
	static DESIRE_FORCE_INLINE Vector4 MinPerElem(const Vector4& vec0, const Vector4& vec1)
	{
		return SIMD::MinPerElem(vec0, vec1);
	}

	// Spherical linear interpolation between two 4-D vectors
	// NOTE: 
	// The result is unpredictable if the vectors point in opposite directions.
	// Does not clamp t between 0 and 1.
	static DESIRE_FORCE_INLINE Vector4 Slerp(float t, const Vector4& unitVec0, const Vector4& unitVec1)
	{
		float scale0, scale1;
		const float cosAngle = unitVec0.Dot(unitVec1);
		if(cosAngle < _VECTORMATH_SLERP_TOL)
		{
			const float angle = std::acosf(cosAngle);
			const float recipSinAngle = 1.0f / std::sinf(angle);
			scale0 = std::sinf((1.0f - t) * angle) * recipSinAngle;
			scale1 = std::sinf(t * angle) * recipSinAngle;
		}
		else
		{
			scale0 = 1.0f - t;
			scale1 = t;
		}
		return unitVec0 * scale0 + unitVec1 * scale1;
	}

	// Construct axis 4-D vector
	static DESIRE_FORCE_INLINE Vector4 AxisX()		{ return Vector4(1.0f, 0.0f, 0.0f, 0.0f); }
	static DESIRE_FORCE_INLINE Vector4 AxisY()		{ return Vector4(0.0f, 1.0f, 0.0f, 0.0f); }
	static DESIRE_FORCE_INLINE Vector4 AxisZ()		{ return Vector4(0.0f, 0.0f, 1.0f, 0.0f); }
	static DESIRE_FORCE_INLINE Vector4 AxisW()		{ return Vector4(0.0f, 0.0f, 0.0f, 1.0f); }

private:
	vec_float4_t mVec128;
};

// Multiply a 4-D vector by a scalar
DESIRE_FORCE_INLINE Vector4 operator *(float scalar, const Vector4& vec)
{
	return vec * scalar;
}
