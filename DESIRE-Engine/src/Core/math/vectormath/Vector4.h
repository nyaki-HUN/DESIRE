#pragma once

class Vector4
{
public:
	inline Vector4()
	{
		// No initialization
	}

	inline Vector4(const Vector4& vec)
		: mVec128(vec.mVec128)
	{
	
	}
	
	inline Vector4(vec_float4_t vf)
		: mVec128(vf) 
	{
	
	}

	inline Vector4(float x, float y, float z, float w)
	{
		SIMD::Construct(mVec128, x, y, z, w);
	}

	explicit inline Vector4(float scalar)
	{
		SIMD::Construct(mVec128, scalar);
	}

	// Construct a 4-D vector from a 3-D vector and a scalar
	inline Vector4(const Vector3& xyz, float w)
	{
		mVec128 = xyz;
		SetW(w);
	}

	// Copy x, y, and z from a 3-D vector into a 4-D vector, and set w to 0
	explicit inline Vector4(const Vector3& vec)
		: Vector4(vec, 0.0f)
	{

	}

	// Load x, y, z, and w elements from the first four elements of a float array
	inline void LoadXYZW(const float *fptr)
	{
		SIMD::LoadXYZW(mVec128, fptr);
	}

	// Store x, y, z, and w elements of a 4-D vector in the first four elements of a float array
	inline void StoreXYZW(float *fptr) const
	{
		SIMD::StoreXYZW(mVec128, fptr);
	}

	// Set the x, y, and z elements of a 4-D vector (does not change the w element)
	inline void SetXYZ(const Vector3& vec)
	{
		mVec128 = SIMD::Blend(vec, mVec128, SIMD::MaskW());
	}

	// Get the x, y, and z elements of a 4-D vector
	inline Vector3 GetXYZ() const
	{
		return mVec128;
	}

	// Set element
	inline void SetX(float x)		{ SIMD::SetX(mVec128, x); }
	inline void SetY(float y)		{ SIMD::SetY(mVec128, y); }
	inline void SetZ(float z)		{ SIMD::SetZ(mVec128, z); }
	inline void SetW(float w)		{ SIMD::SetW(mVec128, w); }

	// Get element
	inline float GetX() const		{ return SIMD::GetX(mVec128); }
	inline float GetY() const		{ return SIMD::GetY(mVec128); }
	inline float GetZ() const		{ return SIMD::GetZ(mVec128); }
	inline float GetW() const		{ return SIMD::GetW(mVec128); }

	// Operator overloads
	inline operator vec_float4_t() const
	{
		return mVec128;
	}

	inline Vector4& operator =(const Vector4& vec)
	{
		mVec128 = vec.mVec128;
		return *this;
	}

	inline Vector4 operator -() const						{ return SIMD::Negate(mVec128); }
	inline Vector4 operator +(const Vector4& vec) const	{ return SIMD::Add(mVec128, vec.mVec128); }
	inline Vector4 operator -(const Vector4& vec) const	{ return SIMD::Sub(mVec128, vec.mVec128); }
	inline Vector4 operator *(float scalar) const			{ return SIMD::Mul(mVec128, scalar); }

	inline Vector4 operator /(float scalar) const
	{
		return (*this * (1.0f / scalar));
	}

	inline Vector4& operator +=(const Vector4& vec)
	{
		*this = *this + vec;
		return *this;
	}

	inline Vector4& operator -=(const Vector4& vec)
	{
		*this = *this - vec;
		return *this;
	}

	inline Vector4& operator *=(float scalar)
	{
		*this = *this * scalar;
		return *this;
	}

	inline Vector4& operator /=(float scalar)
	{
		*this = *this / scalar;
		return *this;
	}

	// Maximum element of a 4-D vector
	inline float GetMaxElem() const
	{
		return SIMD::GetX(SIMD::MaxElem4(mVec128));
	}

	// Minimum element of a 4-D vector
	inline float GetMinElem() const
	{
		return SIMD::GetX(SIMD::MinElem4(mVec128));
	}

	// Compute the dot product of two 4-D vectors
	inline float Dot(const Vector4& vec) const
	{
		return SIMD::GetX(SIMD::Dot4(mVec128, vec));
	}

	// Compute the square of the length of a 4-D vector
	inline float LengthSqr() const
	{
		return Dot(*this);
	}

	// Compute the length of a 4-D vector
	inline float Length() const
	{
		return std::sqrt(LengthSqr());
	}

	// Normalize a 4-D vector
	// NOTE: The result is unpredictable when all elements of vec are at or near zero
	inline void Normalize()
	{
		mVec128 = SIMD::Mul(mVec128, newtonrapson_rsqrt4(SIMD::Dot4(mVec128, mVec128)));
	}

	// Get normalized 4-D vector
	// NOTE: The result is unpredictable when all elements of vec are at or near zero
	inline Vector4 Normalized() const
	{
		return SIMD::Mul(mVec128, newtonrapson_rsqrt4(SIMD::Dot4(mVec128, mVec128)));
	}

	// Multiply two 4-D vectors per element
	inline Vector4 MulPerElem(const Vector4& vec) const
	{
		return SIMD::Mul(mVec128, vec);
	}

	// Divide two 4-D vectors per element
	inline Vector4 DivPerElem(const Vector4& vec) const
	{
		return SIMD::Div(mVec128, vec);
	}

	// Compute the absolute value of a 4-D vector per element
	inline Vector4 AbsPerElem() const
	{
		return SIMD::AbsPerElem(mVec128);
	}

	// Maximum of two 4-D vectors per element
	static inline Vector4 MaxPerElem(const Vector4& vec0, const Vector4& vec1)
	{
		return SIMD::MaxPerElem(vec0, vec1);
	}

	// Minimum of two 4-D vectors per element
	static inline Vector4 MinPerElem(const Vector4& vec0, const Vector4& vec1)
	{
		return SIMD::MinPerElem(vec0, vec1);
	}

	// Spherical linear interpolation between two 4-D vectors
	// NOTE: 
	// The result is unpredictable if the vectors point in opposite directions.
	// Does not clamp t between 0 and 1.
	static inline Vector4 Slerp(float t, const Vector4& unitVec0, const Vector4& unitVec1)
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
	static inline Vector4 AxisX()		{ return Vector4(1.0f, 0.0f, 0.0f, 0.0f); }
	static inline Vector4 AxisY()		{ return Vector4(0.0f, 1.0f, 0.0f, 0.0f); }
	static inline Vector4 AxisZ()		{ return Vector4(0.0f, 0.0f, 1.0f, 0.0f); }
	static inline Vector4 AxisW()		{ return Vector4(0.0f, 0.0f, 0.0f, 1.0f); }

private:
	vec_float4_t mVec128;
};

// Multiply a 4-D vector by a scalar
inline Vector4 operator *(float scalar, const Vector4& vec)
{
	return vec * scalar;
}
