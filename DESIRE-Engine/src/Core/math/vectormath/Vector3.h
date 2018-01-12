#pragma once

class Vector3
{
public:
	DESIRE_FORCE_INLINE Vector3()
	{
		// No initialization
	}

	DESIRE_FORCE_INLINE Vector3(const Vector3& vec)
		: mVec128(vec.mVec128)
	{

	}
	
	DESIRE_FORCE_INLINE Vector3(vec_float3_t vf)
		: mVec128(vf)
	{

	}
	
	DESIRE_FORCE_INLINE Vector3(float x, float y, float z)
	{
		SIMD::Construct(mVec128, x, y, z);
	}
	
	explicit DESIRE_FORCE_INLINE Vector3(float scalar)
	{
		SIMD::Construct(mVec128, scalar);
	}

	// Load x, y, and z elements from the first three elements of a float array
	DESIRE_FORCE_INLINE void LoadXYZ(const float *fptr)
	{
		*this = Vector3(fptr[0], fptr[1], fptr[2]);
	}

	// Store x, y, and z elements in the first three elements of a float array
	DESIRE_FORCE_INLINE void StoreXYZ(float *fptr) const
	{
		fptr[0] = GetX();
		fptr[1] = GetY();
		fptr[2] = GetZ();
	}

	// Set element
	DESIRE_FORCE_INLINE void SetX(float x)		{ SIMD::SetX(mVec128, x); }
	DESIRE_FORCE_INLINE void SetY(float y)		{ SIMD::SetY(mVec128, y); }
	DESIRE_FORCE_INLINE void SetZ(float z)		{ SIMD::SetZ(mVec128, z); }

	// Get element
	DESIRE_FORCE_INLINE float GetX() const		{ return SIMD::GetX(mVec128); }
	DESIRE_FORCE_INLINE float GetY() const		{ return SIMD::GetY(mVec128); }
	DESIRE_FORCE_INLINE float GetZ() const		{ return SIMD::GetZ(mVec128); }

	// Operator overloads
	DESIRE_FORCE_INLINE operator vec_float3_t() const
	{
		return mVec128;
	}

	DESIRE_FORCE_INLINE Vector3& operator =(const Vector3& vec)
	{
		mVec128 = vec.mVec128;
		return *this;
	}

	DESIRE_FORCE_INLINE Vector3 operator -() const						{ return SIMD::Negate(mVec128); }
	DESIRE_FORCE_INLINE Vector3 operator +(const Vector3& vec) const	{ return SIMD::Add(mVec128, vec.mVec128); }
	DESIRE_FORCE_INLINE Vector3 operator -(const Vector3& vec) const	{ return SIMD::Sub(mVec128, vec.mVec128); }
	DESIRE_FORCE_INLINE Vector3 operator *(float scalar) const			{ return SIMD::Mul(mVec128, scalar); }

	DESIRE_FORCE_INLINE Vector3 operator /(float scalar) const
	{
		return (*this * (1.0f / scalar));
	}

	DESIRE_FORCE_INLINE Vector3& operator +=(const Vector3& vec)
	{
		*this = *this + vec;
		return *this;
	}

	DESIRE_FORCE_INLINE Vector3& operator -=(const Vector3& vec)
	{
		*this = *this - vec;
		return *this;
	}

	DESIRE_FORCE_INLINE Vector3& operator *=(float scalar)
	{
		*this = *this * scalar;
		return *this;
	}

	DESIRE_FORCE_INLINE Vector3& operator /=(float scalar)
	{
		*this = *this / scalar;
		return *this;
	}

	// Compare 3-D vector to another 3-D vector for greater-than or equal
	DESIRE_FORCE_INLINE bool operator >=(const Vector3& vec) const
	{
		return SIMD::OpCmpGE(mVec128, vec);
	}

	// Compare 3-D vector to another 3-D vector for less-than or equal
	DESIRE_FORCE_INLINE bool operator <=(const Vector3& vec) const
	{
		return SIMD::OpCmpLE(mVec128, vec);
	}

	// Maximum element of a 3-D vector
	DESIRE_FORCE_INLINE float GetMaxElem() const
	{
		return SIMD::GetX(SIMD::MaxElem3(mVec128));
	}

	// Minimum element of a 3-D vector
	DESIRE_FORCE_INLINE float GetMinElem() const
	{
		return SIMD::GetX(SIMD::MinElem3(mVec128));
	}

	// Compute the dot product of two 3-D vectors
	DESIRE_FORCE_INLINE float Dot(const Vector3& vec) const
	{
		return SIMD::GetX(SIMD::Dot3(mVec128, vec));
	}

	// Compute cross product of two 3-D vectors
	DESIRE_FORCE_INLINE Vector3 Cross(const Vector3& vec) const
	{
		return SIMD::Cross(mVec128, vec);
	}

	// Compute the square of the length of a 3-D vector
	DESIRE_FORCE_INLINE float LengthSqr() const
	{
		return Dot(*this);
	}

	// Compute the length of a 3-D vector
	DESIRE_FORCE_INLINE float Length() const
	{
		return std::sqrtf(LengthSqr());
	}

	// Normalize a 3-D vector
	// NOTE: The result is unpredictable when all elements of vec are at or near zero
	DESIRE_FORCE_INLINE void Normalize()
	{
		mVec128 = SIMD::Mul(mVec128, newtonrapson_rsqrt4(SIMD::Dot3(mVec128, mVec128)));
	}

	// Get normalized 3-D vector
	// NOTE: The result is unpredictable when all elements of vec are at or near zero
	DESIRE_FORCE_INLINE Vector3 Normalized() const
	{
		return SIMD::Mul(mVec128, newtonrapson_rsqrt4(SIMD::Dot3(mVec128, mVec128)));
	}

	// Multiply two 3-D vectors per element
	DESIRE_FORCE_INLINE Vector3 MulPerElem(const Vector3& vec) const
	{
		return SIMD::Mul(mVec128, vec);
	}

	// Divide two 3-D vectors per element
	DESIRE_FORCE_INLINE Vector3 DivPerElem(const Vector3& vec) const
	{
		return SIMD::Div(mVec128, vec);
	}

	// Compute the absolute value of a 3-D vector per element
	DESIRE_FORCE_INLINE Vector3 AbsPerElem() const
	{
		return SIMD::AbsPerElem(mVec128);
	}

	// Maximum of two 3-D vectors per element
	static DESIRE_FORCE_INLINE Vector3 MaxPerElem(const Vector3& vec0, const Vector3& vec1)
	{
		return SIMD::MaxPerElem(vec0, vec1);
	}

	// Minimum of two 3-D vectors per element
	static DESIRE_FORCE_INLINE Vector3 MinPerElem(const Vector3& vec0, const Vector3& vec1)
	{
		return SIMD::MinPerElem(vec0, vec1);
	}

	// Spherical linear interpolation between two 3-D vectors
	// NOTE: 
	// The result is unpredictable if the vectors point in opposite directions.
	// Does not clamp t between 0 and 1.
	static DESIRE_FORCE_INLINE Vector3 Slerp(float t, const Vector3& unitVec0, const Vector3& unitVec1)
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

	// Construct axis 3-D vector
	static DESIRE_FORCE_INLINE Vector3 AxisX()		{ return Vector3(1.0f, 0.0f, 0.0f); }
	static DESIRE_FORCE_INLINE Vector3 AxisY()		{ return Vector3(0.0f, 1.0f, 0.0f); }
	static DESIRE_FORCE_INLINE Vector3 AxisZ()		{ return Vector3(0.0f, 0.0f, 1.0f); }

private:
	vec_float3_t mVec128;
};

// Multiply a 3-D vector by a scalar
DESIRE_FORCE_INLINE Vector3 operator *(float scalar, const Vector3& vec)
{
	return vec * scalar;
}
