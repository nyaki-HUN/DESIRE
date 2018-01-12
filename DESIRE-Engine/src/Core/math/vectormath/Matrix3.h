#pragma once

class Matrix3
{
public:
	DESIRE_FORCE_INLINE Matrix3()
	{
		// No initialization
	}

	// Copy a 3x3 matrix
	DESIRE_FORCE_INLINE Matrix3(const Matrix3& mat)
		: col0(mat.col0)
		, col1(mat.col1)
		, col2(mat.col2)
	{

	}

	// Construct a 3x3 matrix containing the specified columns
	DESIRE_FORCE_INLINE Matrix3(const Vector3& col0, const Vector3& col1, const Vector3& col2)
		: col0(col0)
		, col1(col1)
		, col2(col2)
	{

	}

	// Construct a 3x3 rotation matrix from a unit-length quaternion
	explicit DESIRE_FORCE_INLINE Matrix3(const Quat& unitQuat)
	{
#if defined(DESIRE_USE_SSE)
		const __m128 xyzw_2 = SIMD::Add(unitQuat, unitQuat);
		const __m128 wwww = SIMD::Swizzle_WWWW(unitQuat);
		const __m128 yzxw = SIMD::Swizzle_YZXW(unitQuat);
		const __m128 zxyw = SIMD::Swizzle_ZXYW(unitQuat);
		const __m128 yzxw_2 = SIMD::Swizzle_YZXW(xyzw_2);
		const __m128 zxyw_2 = SIMD::Swizzle_ZXYW(xyzw_2);

		__m128 tmp0 = SIMD::Mul(yzxw_2, wwww);								// tmp0 = 2yw, 2zw, 2xw, 2w2
		__m128 tmp1 = SIMD::Sub(_mm_set1_ps(1.0f), SIMD::Mul(yzxw, yzxw_2));	// tmp1 = 1 - 2y2, 1 - 2z2, 1 - 2x2, 1 - 2w2
		__m128 tmp2 = SIMD::Mul(yzxw, xyzw_2);								// tmp2 = 2xy, 2yz, 2xz, 2w2
		tmp0 = SIMD::Add(SIMD::Mul(zxyw, xyzw_2), tmp0);						// tmp0 = 2yw + 2zx, 2zw + 2xy, 2xw + 2yz, 2w2 + 2w2
		tmp1 = SIMD::Sub(tmp1, SIMD::Mul(zxyw, zxyw_2));						// tmp1 = 1 - 2y2 - 2z2, 1 - 2z2 - 2x2, 1 - 2x2 - 2y2, 1 - 2w2 - 2w2
		tmp2 = SIMD::Sub(tmp2, SIMD::Mul(zxyw_2, wwww));						// tmp2 = 2xy - 2zw, 2yz - 2xw, 2xz - 2yw, 2w2 - 2w2

		const __m128 mask_x = SIMD::MaskX();
		const __m128 tmp3 = SIMD::Blend(tmp0, tmp1, mask_x);
		const __m128 tmp4 = SIMD::Blend(tmp1, tmp2, mask_x);
		const __m128 tmp5 = SIMD::Blend(tmp2, tmp0, mask_x);
		const __m128 mask_z = SIMD::MaskZ();
		col0 = SIMD::Blend(tmp3, tmp2, mask_z);
		col1 = SIMD::Blend(tmp4, tmp0, mask_z);
		col2 = SIMD::Blend(tmp5, tmp1, mask_z);
#else
		const float qx = unitQuat.GetX();
		const float qy = unitQuat.GetY();
		const float qz = unitQuat.GetZ();
		const float qw = unitQuat.GetW();
		const float qx2 = qx + qx;
		const float qy2 = qy + qy;
		const float qz2 = qz + qz;
		const float qxqx2 = qx * qx2;
		const float qxqy2 = qx * qy2;
		const float qxqz2 = qx * qz2;
		const float qxqw2 = qw * qx2;
		const float qyqy2 = qy * qy2;
		const float qyqz2 = qy * qz2;
		const float qyqw2 = qw * qy2;
		const float qzqz2 = qz * qz2;
		const float qzqw2 = qw * qz2;
		col0 = Vector3(1.0f - qyqy2 - qzqz2, qxqy2 + qzqw2, qxqz2 - qyqw2);
		col1 = Vector3(qxqy2 - qzqw2, (1.0f - qxqx2) - qzqz2, qyqz2 + qxqw2);
		col2 = Vector3(qxqz2 + qyqw2, qyqz2 - qxqw2, (1.0f - qxqx2) - qyqy2);
#endif
	}

	// Set the column of a 3x3 matrix referred to by the specified index
	DESIRE_FORCE_INLINE Matrix3& SetCol(int idx, const Vector3& vec)
	{
		*(&col0 + idx) = vec;
		return *this;
	}

	// Get the column of a 3x3 matrix referred to by the specified index
	DESIRE_FORCE_INLINE const Vector3& GetCol(int idx) const
	{
		return *(&col0 + idx);
	}

	// Set the first row of a 3x3 matrix
	DESIRE_FORCE_INLINE void SetRow0(const Vector3& vec)
	{
		col0.SetX(vec.GetX());
		col1.SetX(vec.GetY());
		col2.SetX(vec.GetZ());
	}

	// Get the first row of a 3x3 matrix
	DESIRE_FORCE_INLINE Vector3 GetRow0() const
	{
		return Vector3(col0.GetX(), col1.GetX(), col2.GetX());
	}

	// Get the second row of a 3x3 matrix
	DESIRE_FORCE_INLINE Vector3 GetRow1() const
	{
		return Vector3(col0.GetY(), col1.GetY(), col2.GetY());
	}

	// Get the third row of a 3x3 matrix
	DESIRE_FORCE_INLINE Vector3 GetRow2() const
	{
		return Vector3(col0.GetZ(), col1.GetZ(), col2.GetZ());
	}

	// Assign one 3x3 matrix to another
	DESIRE_FORCE_INLINE Matrix3& operator =(const Matrix3& mat)
	{
		col0 = mat.col0;
		col1 = mat.col1;
		col2 = mat.col2;
		return *this;
	}

	// Negate all elements of a 3x3 matrix
	DESIRE_FORCE_INLINE Matrix3 operator -() const
	{
		return Matrix3(-col0, -col1, -col2);
	}

	// Add two 3x3 matrices
	DESIRE_FORCE_INLINE Matrix3 operator +(const Matrix3& mat) const
	{
		return Matrix3(
			col0 + mat.col0,
			col1 + mat.col1,
			col2 + mat.col2
		);
	}

	// Subtract a 3x3 matrix from another 3x3 matrix
	DESIRE_FORCE_INLINE Matrix3 operator -(const Matrix3& mat) const
	{
		return Matrix3(
			col0 - mat.col0,
			col1 - mat.col1,
			col2 - mat.col2
		);
	}

	// Multiply a 3x3 matrix by a scalar
	DESIRE_FORCE_INLINE Matrix3 operator *(float scalar) const
	{
		return Matrix3(col0 * scalar, col1 * scalar, col2 * scalar);
	}

	// Multiply a 3x3 matrix by a 3-D vector
	DESIRE_FORCE_INLINE Vector3 operator *(const Vector3& vec) const
	{
#if defined(DESIRE_USE_SSE)
		vec_float3_t result;
		result = SIMD::Mul(col0, SIMD::Swizzle_XXXX(vec));
		result = SIMD::MulAdd(col1, SIMD::Swizzle_YYYY(vec), result);
		result = SIMD::MulAdd(col2, SIMD::Swizzle_ZZZZ(vec), result);
		return result;
#elif defined(__ARM_NEON__)
		float32x4_t result;
		const float32x2_t vecLow = vget_low_f32(vec);
		result = vmulq_lane_f32(col0, vecLow, 0);
		result = vmlaq_lane_f32(result, col1, vecLow, 1);
		const float32x2_t vecHigh = vget_high_f32(vec);
		result = vmlaq_lane_f32(result, col2, vecHigh, 0);
		return result;
#else
		return Vector3(
			col0.GetX() * vec.GetX() + col1.GetX() * vec.GetY() + col2.GetX() * vec.GetZ(),
			col0.GetY() * vec.GetX() + col1.GetY() * vec.GetY() + col2.GetY() * vec.GetZ(),
			col0.GetZ() * vec.GetX() + col1.GetZ() * vec.GetY() + col2.GetZ() * vec.GetZ()
		);
#endif
	}

	// Multiply two 3x3 matrices
	DESIRE_FORCE_INLINE Matrix3 operator *(const Matrix3& mat) const
	{
		return Matrix3(
			*this * mat.col0,
			*this * mat.col1,
			*this * mat.col2
		);
	}

	// Perform compound assignment and addition with a 3x3 matrix
	DESIRE_FORCE_INLINE Matrix3& operator +=(const Matrix3& mat)
	{
		*this = *this + mat;
		return *this;
	}

	// Perform compound assignment and subtraction by a 3x3 matrix
	DESIRE_FORCE_INLINE Matrix3& operator -=(const Matrix3& mat)
	{
		*this = *this - mat;
		return *this;
	}

	// Perform compound assignment and multiplication by a scalar
	DESIRE_FORCE_INLINE Matrix3& operator *=(float scalar)
	{
		*this = *this * scalar;
		return *this;
	}

	// Perform compound assignment and multiplication by a 3x3 matrix
	DESIRE_FORCE_INLINE Matrix3& operator *=(const Matrix3& mat)
	{
		*this = *this * mat;
		return *this;
	}

	// Append (post-multiply) a scale transformation to a 3x3 matrix
	// NOTE: Faster than creating and multiplying a scale transformation matrix.
	DESIRE_FORCE_INLINE Matrix3& AppendScale(const Vector3& scaleVec)
	{
		col0 *= scaleVec.GetX();
		col1 *= scaleVec.GetY();
		col2 *= scaleVec.GetZ();
		return *this;
	}

	// Prepend (pre-multiply) a scale transformation to a 3x3 matrix
	// NOTE: Faster than creating and multiplying a scale transformation matrix.
	DESIRE_FORCE_INLINE Matrix3& PrependScale(const Vector3& scaleVec)
	{
		col0 = col0.MulPerElem(scaleVec);
		col1 = col1.MulPerElem(scaleVec);
		col2 = col2.MulPerElem(scaleVec);
		return *this;
	}

	// Transpose a 3x3 matrix
	DESIRE_FORCE_INLINE void Transpose()
	{
#if defined(DESIRE_USE_SSE)
		const __m128 mask_y = SIMD::MaskY();

		__m128 tmp0 = _mm_unpacklo_ps(col0, col2);
		__m128 tmp1 = _mm_unpackhi_ps(col0, col2);
		col0 = _mm_unpacklo_ps(tmp0, col1);

		tmp1 = _mm_shuffle_ps(tmp1, tmp1, _MM_SHUFFLE(0, 1, 1, 0));
		col2 = SIMD::Blend(tmp1, SIMD::Swizzle_ZZZZ(col1), mask_y);

		tmp0 = _mm_shuffle_ps(tmp0, tmp0, _MM_SHUFFLE(0, 3, 2, 2));
		col1 = SIMD::Blend(tmp0, col1, mask_y);
#else
		const Vector3 tmp0(col0.GetX(), col1.GetX(), col2.GetX());
		const Vector3 tmp1(col0.GetY(), col1.GetY(), col2.GetY());
		const Vector3 tmp2(col0.GetZ(), col1.GetZ(), col2.GetZ());
		col0 = tmp0;
		col1 = tmp1;
		col2 = tmp2;
#endif
	}

	// Compute the inverse of a 3x3 matrix
	// NOTE: Result is unpredictable when the determinant of mat is equal to or near 0
	DESIRE_FORCE_INLINE void Invert();

	// Determinant of a 3x3 matrix
	DESIRE_FORCE_INLINE float CalculateDeterminant() const
	{
		return col2.Dot(col0.Cross(col1));
	}

	// Construct an identity 3x3 matrix
	static DESIRE_FORCE_INLINE Matrix3 Identity()
	{
		return Matrix3(
			Vector3::AxisX(),
			Vector3::AxisY(),
			Vector3::AxisZ()
		);
	}

	// Construct a 3x3 matrix to rotate around the x axis
	static DESIRE_FORCE_INLINE Matrix3 CreateRotationX(float radians);

	// Construct a 3x3 matrix to rotate around the y axis
	static DESIRE_FORCE_INLINE Matrix3 CreateRotationY(float radians);

	// Construct a 3x3 matrix to rotate around the z axis
	static DESIRE_FORCE_INLINE Matrix3 CreateRotationZ(float radians);

	// Construct a 3x3 matrix to rotate around the x, y, and z axes
	static DESIRE_FORCE_INLINE Matrix3 CreateRotationZYX(const Vector3& radiansXYZ);

	// Construct a 3x3 matrix to rotate around a unit-length 3-D vector
	static DESIRE_FORCE_INLINE Matrix3 CreateRotation(float radians, const Vector3& unitVec);

	// Construct a rotation matrix from a unit-length quaternion
	static DESIRE_FORCE_INLINE Matrix3 CreateRotation(const Quat& unitQuat)
	{
		return Matrix3(unitQuat);
	}

	// Construct a 3x3 matrix to perform scaling
	static DESIRE_FORCE_INLINE Matrix3 CreateScale(const Vector3& scaleVec)
	{
#if defined(DESIRE_USE_SSE) || defined(__ARM_NEON__)
		const Vector3 zero(0.0f);
		return Matrix3(
			SIMD::Blend(zero, scaleVec, SIMD::MaskX()),
			SIMD::Blend(zero, scaleVec, SIMD::MaskY()),
			SIMD::Blend(zero, scaleVec, SIMD::MaskZ())
		);
#else
		return Matrix3(
			Vector3(scaleVec.GetX(), 0.0f, 0.0f),
			Vector3(0.0f, scaleVec.GetY(), 0.0f),
			Vector3(0.0f, 0.0f, scaleVec.GetZ())
		);
#endif
	}

	Vector3 col0;
	Vector3 col1;
	Vector3 col2;
};

// Multiply a 3x3 matrix by a scalar
DESIRE_FORCE_INLINE Matrix3 operator *(float scalar, const Matrix3& mat)
{
	return mat * scalar;
}
