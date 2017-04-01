#pragma once

class Matrix3;

// --------------------------------------------------------------------------------------------------------------------
//	Quaternion
// --------------------------------------------------------------------------------------------------------------------

class Quat
{
public:
	// Constructors
	DESIRE_FORCE_INLINE Quat() {}	// No initialization
	DESIRE_FORCE_INLINE Quat(const Quat& vec)						: mVec128(vec.mVec128) {}
	DESIRE_FORCE_INLINE Quat(vec_float4_t vf4)						: mVec128(vf4) {}
	DESIRE_FORCE_INLINE Quat(float x, float y, float z, float w)	{ SIMD::Construct(mVec128, x, y, z, w); }

	// Construct a quaternion from a 3-D vector and a scalar
	DESIRE_FORCE_INLINE Quat(const Vector3& xyz, float w)
	{
		mVec128 = xyz;
		SetW(w);
	}

	// Convert a rotation matrix to a unit-length quaternion
	explicit DESIRE_FORCE_INLINE Quat(const Matrix3& rotMat);

	// Load x, y, z, and w elements from the first four words of a float array
	DESIRE_FORCE_INLINE void LoadXYZW(const float *fptr)
	{
		SIMD::LoadXYZW(mVec128, fptr);
	}

	// Store x, y, z, and w elements of a quaternion in the first four words of a float array
	DESIRE_FORCE_INLINE void StoreXYZW(float *fptr) const
	{
		SIMD::StoreXYZW(mVec128, fptr);
	}

	// Set the x, y, and z elements of a quaternion (does not change the w element)
	DESIRE_FORCE_INLINE void SetXYZ(const Vector3& vec)
	{
		SIMD::SetXYZ(mVec128, vec);
	}

	// Get the x, y, and z elements of a quaternion
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

	DESIRE_FORCE_INLINE Quat& operator =(const Quat& quat)
	{
		mVec128 = quat.mVec128;
		return *this;
	}

	DESIRE_FORCE_INLINE Quat operator -() const						{ return SIMD::Negate(mVec128); }
	DESIRE_FORCE_INLINE Quat operator +(const Quat& quat) const		{ return SIMD::Add(mVec128, quat.mVec128); }
	DESIRE_FORCE_INLINE Quat operator -(const Quat& quat) const		{ return SIMD::Sub(mVec128, quat.mVec128); }
	DESIRE_FORCE_INLINE Quat operator *(float scalar) const			{ return SIMD::Mul(mVec128, scalar); }

	DESIRE_FORCE_INLINE Quat operator *(const Quat& quat) const;

	DESIRE_FORCE_INLINE Quat operator /(float scalar) const
	{
		return (*this * (1.0f / scalar));
	}

	DESIRE_FORCE_INLINE Quat& operator +=(const Quat& quat)
	{
		*this = *this + quat;
		return *this;
	}

	DESIRE_FORCE_INLINE Quat& operator -=(const Quat& quat)
	{
		*this = *this - quat;
		return *this;
	}

	DESIRE_FORCE_INLINE Quat& operator *=(float scalar)
	{
		*this = *this * scalar;
		return *this;
	}

	DESIRE_FORCE_INLINE Quat& operator *=(const Quat& quat)
	{
		*this = *this * quat;
		return *this;
	}

	DESIRE_FORCE_INLINE Quat& operator /=(float scalar)
	{
		*this = *this / scalar;
		return *this;
	}

	// Compute the dot product of two quaternions
	DESIRE_FORCE_INLINE float Dot(const Quat& quat) const
	{
		return SIMD::GetX(SIMD::Dot4(mVec128, quat));
	}

	// Compute the norm of a quaternion
	DESIRE_FORCE_INLINE float Norm() const
	{
		return Dot(*this);
	}

	// Compute the length of a quaternion
	DESIRE_FORCE_INLINE float Length() const
	{
		return sqrtf(Norm());
	}

	// Compute the conjugate of a quaternion
	DESIRE_FORCE_INLINE Quat Conjugate() const
	{
		return SIMD::Conjugate(mVec128);
	}

	// Rotate a 3-D vector with this unit-length quaternion
	DESIRE_FORCE_INLINE Vector3 RotateVec(const Vector3& vec) const;

	// Normalize a quaternion
	// NOTE: The result is unpredictable when all elements of quat are at or near zero.
	DESIRE_FORCE_INLINE Quat& Normalize()
	{
		mVec128 = SIMD::MulPerElem(mVec128, newtonrapson_rsqrt4(SIMD::Dot4(mVec128, mVec128)));
		return *this;
	}

	// Spherical linear interpolation between two quaternions
	// NOTE: 
	// Interpolates along the shortest path between orientations
	// Does not clamp t between 0 and 1.
	static DESIRE_FORCE_INLINE Quat Slerp(float t, const Quat& unitQuat0, const Quat& unitQuat1);

	// Spherical quadrangle interpolation
	static DESIRE_FORCE_INLINE Quat Squad(float t, const Quat& unitQuat0, const Quat& unitQuat1, const Quat& unitQuat2, const Quat& unitQuat3)
	{
		return Quat::Slerp((2.0f * t) * (1.0f - t), Quat::Slerp(t, unitQuat0, unitQuat3), Quat::Slerp(t, unitQuat1, unitQuat2));
	}

	// Construct an identity quaternion
	static DESIRE_FORCE_INLINE Quat Identity()
	{
		return Quat(0.0f, 0.0f, 0.0f, 1.0f);
	}

	// Construct a quaternion to rotate between two unit-length 3-D vectors
	// NOTE: The result is unpredictable if unitVec0 and unitVec1 point in opposite directions.
	static DESIRE_FORCE_INLINE Quat CreateRotation(const Vector3& unitVec0, const Vector3& unitVec1);

	// Construct a quaternion to rotate around a unit-length 3-D vector
	static DESIRE_FORCE_INLINE Quat CreateRotation(float radians, const Vector3& unitVec);

	// Construct a quaternion to rotate around the x axis
	static DESIRE_FORCE_INLINE Quat CreateRotationX(float radians);

	// Construct a quaternion to rotate around the y axis
	static DESIRE_FORCE_INLINE Quat CreateRotationY(float radians);

	// Construct a quaternion to rotate around the z axis
	static DESIRE_FORCE_INLINE Quat CreateRotationZ(float radians);

private:
	vec_float4_t mVec128;
};

// Multiply a quaternion by a scalar
DESIRE_FORCE_INLINE Quat operator *(float scalar, const Quat& quat)
{
	return quat * scalar;
}
