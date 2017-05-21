#pragma once

// --------------------------------------------------------------------------------------------------------------------
//	Matrix4
// --------------------------------------------------------------------------------------------------------------------

class Matrix4
{
public:
	// Default constructor; does no initialization
	DESIRE_FORCE_INLINE Matrix4() {}

	// Copy a 4x4 matrix
	DESIRE_FORCE_INLINE Matrix4(const Matrix4& mat)
		: col0(mat.col0)
		, col1(mat.col1)
		, col2(mat.col2)
		, col3(mat.col3)
	{

	}

	// Construct a 4x4 matrix containing the specified columns
	DESIRE_FORCE_INLINE Matrix4(const Vector4& col0, const Vector4& col1, const Vector4& col2, const Vector4& col3)
		: col0(col0)
		, col1(col1)
		, col2(col2)
		, col3(col3)
	{

	}

	// Construct a 4x4 matrix from a 3x3 matrix and a 3-D vector
	DESIRE_FORCE_INLINE Matrix4(const Matrix3& mat, const Vector3& translateVec)
		: col0(mat.col0, 0.0f)
		, col1(mat.col1, 0.0f)
		, col2(mat.col2, 0.0f)
		, col3(translateVec, 1.0f)
	{

	}

	// Construct a 4x4 matrix from a unit-length quaternion and a 3-D vector
	DESIRE_FORCE_INLINE Matrix4(const Quat& unitQuat, const Vector3& translateVec)
	{
		Matrix3 mat = Matrix3(unitQuat);
		col0 = Vector4(mat.col0, 0.0f);
		col1 = Vector4(mat.col1, 0.0f);
		col2 = Vector4(mat.col2, 0.0f);
		col3 = Vector4(translateVec, 1.0f);
	}

	// Set elements of a 4x4 matrix from a 16 element float array
	DESIRE_FORCE_INLINE Matrix4(const float(&fptr)[16])
	{
		col0.LoadXYZW(&fptr[0]);
		col1.LoadXYZW(&fptr[4]);
		col2.LoadXYZW(&fptr[8]);
		col3.LoadXYZW(&fptr[12]);
	}

	// Store elements of a 4x4 matrix in a 16 elements float array
	DESIRE_FORCE_INLINE void Store(float(&fptr)[16]) const
	{
		col0.StoreXYZW(&fptr[0]);
		col1.StoreXYZW(&fptr[4]);
		col2.StoreXYZW(&fptr[8]);
		col3.StoreXYZW(&fptr[12]);
	}

	// Set the upper-left 3x3 submatrix
	// NOTE: This function does not change the bottom row elements.
	DESIRE_FORCE_INLINE Matrix4& SetUpper3x3(const Matrix3& mat3)
	{
		col0.SetXYZ(mat3.col0);
		col1.SetXYZ(mat3.col1);
		col2.SetXYZ(mat3.col2);
		return *this;
	}

	// Get the upper-left 3x3 submatrix of a 4x4 matrix
	DESIRE_FORCE_INLINE Matrix3 GetUpper3x3() const
	{
		return Matrix3(
			col0.GetXYZ(),
			col1.GetXYZ(),
			col2.GetXYZ()
		);
	}

	// Set translation component
	// NOTE: This function does not change the bottom row elements.
	DESIRE_FORCE_INLINE Matrix4& SetTranslation(const Vector3& translateVec)
	{
		col3.SetXYZ(translateVec);
		return *this;
	}

	// Get the translation component of a 4x4 matrix
	DESIRE_FORCE_INLINE Vector3 GetTranslation() const
	{
		return col3.GetXYZ();
	}

	// Set the column of a 4x4 matrix referred to by the specified index
	DESIRE_FORCE_INLINE Matrix4& SetCol(int idx, const Vector4& vec)
	{
		*(&col0 + idx) = vec;
		return *this;
	}

	// Get the column of a 4x4 matrix referred to by the specified index
	DESIRE_FORCE_INLINE const Vector4& GetCol(int idx) const
	{
		return *(&col0 + idx);
	}

	// Set the row of a 4x4 matrix referred to by the specified index
	DESIRE_FORCE_INLINE void SetRow0(const Vector4& vec)
	{
		col0.SetX(vec.GetX());
		col1.SetX(vec.GetY());
		col2.SetX(vec.GetZ());
		col3.SetX(vec.GetW());
	}

	// Get the row of a 4x4 matrix
	DESIRE_FORCE_INLINE Vector4 GetRow0() const
	{
		return Vector4(col0.GetX(), col1.GetX(), col2.GetX(), col3.GetX());
	}

	// Assign one 4x4 matrix to another
	DESIRE_FORCE_INLINE Matrix4& operator =(const Matrix4& mat)
	{
		col0 = mat.col0;
		col1 = mat.col1;
		col2 = mat.col2;
		col3 = mat.col3;
		return *this;
	}

	// Negate all elements of a 4x4 matrix
	DESIRE_FORCE_INLINE Matrix4 operator -() const
	{
		return Matrix4(-col0, -col1, -col2, -col3);
	}

	// Add two 4x4 matrices
	DESIRE_FORCE_INLINE Matrix4 operator +(const Matrix4& mat) const
	{
		return Matrix4(
			col0 + mat.col0,
			col1 + mat.col1,
			col2 + mat.col2,
			col3 + mat.col3
		);
	}

	// Subtract a 4x4 matrix from another 4x4 matrix
	DESIRE_FORCE_INLINE Matrix4 operator -(const Matrix4& mat) const
	{
		return Matrix4(
			col0 - mat.col0,
			col1 - mat.col1,
			col2 - mat.col2,
			col3 - mat.col3
		);
	}

	// Multiply a 4x4 matrix by a scalar
	DESIRE_FORCE_INLINE Matrix4 operator *(float scalar) const
	{
		return Matrix4(col0 * scalar, col1 * scalar, col2 * scalar, col3 * scalar);
	}

	// Multiply a 4x4 matrix by a 4-D vector
	DESIRE_FORCE_INLINE Vector4 operator *(const Vector4& vec) const;

	// Multiply a 4x4 matrix by a 3-D vector
	DESIRE_FORCE_INLINE Vector4 operator *(const Vector3& vec) const;

	// Multiply two 4x4 matrices
	DESIRE_FORCE_INLINE Matrix4 operator *(const Matrix4& mat) const
	{
		return Matrix4(
			*this * mat.col0,
			*this * mat.col1,
			*this * mat.col2,
			*this * mat.col3
		);
	}

	// Perform compound assignment and addition with a 4x4 matrix
	DESIRE_FORCE_INLINE Matrix4& operator +=(const Matrix4& mat)
	{
		*this = *this + mat;
		return *this;
	}

	// Perform compound assignment and subtraction by a 4x4 matrix
	DESIRE_FORCE_INLINE Matrix4& operator -=(const Matrix4& mat)
	{
		*this = *this - mat;
		return *this;
	}

	// Perform compound assignment and multiplication by a scalar
	DESIRE_FORCE_INLINE Matrix4& operator *=(float scalar)
	{
		*this = *this * scalar;
		return *this;
	}

	// Perform compound assignment and multiplication by a 4x4 matrix
	DESIRE_FORCE_INLINE Matrix4& operator *=(const Matrix4& mat)
	{
		*this = *this * mat;
		return *this;
	}

	// Append (post-multiply) a scale transformation to a 4x4 matrix
	// NOTE: Faster than creating and multiplying a scale transformation matrix
	DESIRE_FORCE_INLINE Matrix4& AppendScale(const Vector3& scaleVec)
	{
		col0 *= scaleVec.GetX();
		col1 *= scaleVec.GetY();
		col2 *= scaleVec.GetZ();
		return *this;
	}

	// Prepend (pre-multiply) a scale transformation to a 4x4 matrix
	// NOTE: Faster than creating and multiplying a scale transformation matrix
	DESIRE_FORCE_INLINE Matrix4& PrependScale(const Vector3& scaleVec)
	{
		const Vector4 scale4(scaleVec, 1.0f);
		col0 = col0.MulPerElem(scale4);
		col1 = col1.MulPerElem(scale4);
		col2 = col2.MulPerElem(scale4);
		col3 = col3.MulPerElem(scale4);
		return *this;
	}

	// Transpose a 4x4 matrix
	DESIRE_FORCE_INLINE void Transpose();

	// Compute the inverse of a 4x4 matrix
	// NOTE: Result is unpredictable when the determinant of mat is equal to or near 0
	DESIRE_FORCE_INLINE void Invert();

	// Compute the inverse of a 4x4 matrix, which is expected to be an affine matrix
	// NOTE: This can be used to achieve better performance than a general inverse when the specified 4x4 matrix meets the given restrictions. The result is unpredictable when the determinant of mat is equal to or near 0
	DESIRE_FORCE_INLINE void AffineInvert();

	// Compute the inverse of a 4x4 matrix, which is expected to be an affine matrix with an orthogonal upper-left 3x3 submatrix
	// NOTE: This can be used to achieve better performance than a general inverse when the specified 4x4 matrix meets the given restrictions
	DESIRE_FORCE_INLINE void OrthoInvert();

	// Determinant of a 4x4 matrix
	DESIRE_FORCE_INLINE float CalculateDeterminant() const;

	// Construct an identity 4x4 matrix
	static DESIRE_FORCE_INLINE Matrix4 Identity()
	{
		return Matrix4(
			Vector4::AxisX(),
			Vector4::AxisY(),
			Vector4::AxisZ(),
			Vector4::AxisW()
		);
	}

	// Construct a 4x4 matrix to perform translation
	static DESIRE_FORCE_INLINE Matrix4 CreateTranslation(const Vector3& translateVec)
	{
		return Matrix4(
			Vector4::AxisX(),
			Vector4::AxisY(),
			Vector4::AxisZ(),
			Vector4(translateVec, 1.0f)
		);
	}

	// Construct a 4x4 matrix to rotate around the x axis
	static DESIRE_FORCE_INLINE Matrix4 CreateRotationX(float radians);

	// Construct a 4x4 matrix to rotate around the y axis
	static DESIRE_FORCE_INLINE Matrix4 CreateRotationY(float radians);

	// Construct a 4x4 matrix to rotate around the z axis
	static DESIRE_FORCE_INLINE Matrix4 CreateRotationZ(float radians);

	// Construct a 4x4 matrix to rotate around the x, y, and z axes
	static DESIRE_FORCE_INLINE Matrix4 CreateRotationZYX(const Vector3& radiansXYZ);

	// Construct a 4x4 matrix to rotate around a unit-length 3-D vector
	static DESIRE_FORCE_INLINE Matrix4 CreateRotation(float radians, const Vector3& unitVec);

	// Construct a rotation matrix from a unit-length quaternion
	static DESIRE_FORCE_INLINE Matrix4 CreateRotation(const Quat& unitQuat)
	{
		return Matrix4(unitQuat, Vector3(0.0f));
	}

	// Construct a 4x4 matrix to perform scaling
	static DESIRE_FORCE_INLINE Matrix4 CreateScale(const Vector3& scaleVec);

	Vector4 col0;
	Vector4 col1;
	Vector4 col2;
	Vector4 col3;
};

// Multiply a 4x4 matrix by a scalar
DESIRE_FORCE_INLINE Matrix4 operator *(float scalar, const Matrix4& mat)
{
	return mat * scalar;
}
