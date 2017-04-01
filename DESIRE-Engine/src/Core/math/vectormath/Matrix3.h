#pragma once

// --------------------------------------------------------------------------------------------------------------------
//	Matrix3
// --------------------------------------------------------------------------------------------------------------------

class Matrix3
{
public:
	// Default constructor; does no initialization
	DESIRE_FORCE_INLINE Matrix3() {}

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

	// Set all elements of a 3x3 matrix to the same scalar value
	explicit DESIRE_FORCE_INLINE Matrix3(float scalar)
		: col0(scalar)
		, col1(scalar)
		, col2(scalar)
	{

	}

	// Construct a 3x3 rotation matrix from a unit-length quaternion
	explicit DESIRE_FORCE_INLINE Matrix3(const Quat& unitQuat);

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

	// Set the row of a 3x3 matrix referred to by the specified index
	DESIRE_FORCE_INLINE void SetRow0(const Vector3& vec)
	{
		col0.SetX(vec.GetX());
		col1.SetX(vec.GetY());
		col2.SetX(vec.GetZ());
	}

	// Get the row of a 3x3 matrix
	DESIRE_FORCE_INLINE Vector3 GetRow0() const
	{
		return Vector3(col0.GetX(), col1.GetX(), col2.GetX());
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
	DESIRE_FORCE_INLINE Vector3 operator *(const Vector3& vec) const;

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
	DESIRE_FORCE_INLINE void Transpose();

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
	static DESIRE_FORCE_INLINE Matrix3 CreateScale(const Vector3& scaleVec);

	Vector3 col0;
	Vector3 col1;
	Vector3 col2;
};

// Multiply a 3x3 matrix by a scalar
DESIRE_FORCE_INLINE Matrix3 operator *(float scalar, const Matrix3& mat)
{
	return mat * scalar;
}
