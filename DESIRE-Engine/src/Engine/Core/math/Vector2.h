#pragma once

#include <cmath>

class Vector2
{
public:
	inline Vector2()
	{
		// No initialization
	}

	inline Vector2(const Vector2& vec)
		: x(vec.x)
		, y(vec.y)
	{

	}

	inline Vector2(float x, float y)
		: x(x)
		, y(y)
	{

	}

	explicit inline Vector2(float scalar)
		: x(scalar)
		, y(scalar)
	{

	}

	// Set element
	inline void SetX(float _x)		{ x = _x; }
	inline void SetY(float _y)		{ y = _y; }

	// Get element
	inline float GetX() const		{ return x; }
	inline float GetY() const		{ return y; }

	inline Vector2& operator =(const Vector2& vec)
	{
		x = vec.x;
		y = vec.y;
		return *this;
	}

	inline Vector2 operator -() const						{ return Vector2(-x, -y); }
	inline Vector2 operator +(const Vector2& vec) const		{ return Vector2(x + vec.x, y + vec.y); }
	inline Vector2 operator -(const Vector2& vec) const		{ return Vector2(x - vec.x, y - vec.y); }
	inline Vector2 operator *(const Vector2& vec) const		{ return Vector2(x * vec.x, y * vec.y); }
	inline Vector2 operator *(float scalar) const			{ return Vector2(x * scalar, y * scalar); }
	inline Vector2 operator /(const Vector2& vec) const		{ return Vector2(x / vec.x, y / vec.y); }
	inline Vector2 operator /(float scalar) const			{ return Vector2(x / scalar, y / scalar); }

	inline Vector2& operator +=(const Vector2& vec)			{ *this = *this + vec;		return *this; }
	inline Vector2& operator -=(const Vector2& vec)			{ *this = *this - vec;		return *this; }
	inline Vector2& operator *=(const Vector2& vec)			{ *this = *this * vec;		return *this; }
	inline Vector2& operator *=(float scalar)				{ *this = *this * scalar;	return *this; }
	inline Vector2& operator /=(const Vector2& vec)			{ *this = *this / vec;		return *this; }
	inline Vector2& operator /=(float scalar)				{ *this = *this / scalar;	return *this; }

	inline bool operator >=(const Vector2& vec) const
	{
		return (x >= vec.x && y >= vec.y);
	}

	inline bool operator <=(const Vector2& vec) const
	{
		return (x <= vec.x && y <= vec.y);
	}

	// Maximum element of a 2-D vector
	inline float GetMaxElem()
	{
		return (x > y) ? x : y;
	}

	// Minimum element of a 2-D vector
	inline float GetMinElem()
	{
		return (x < y) ? x : y;
	}

	// Compute the dot product of two 2-D vectors
	inline float Dot(const Vector2& vec) const
	{
		return x * vec.x + y * vec.y;
	}

	// Compute the square of the length of a 2-D vector
	inline float LengthSqr() const
	{
		return Dot(*this);
	}

	// Compute the length of a 2-D vector
	inline float Length() const
	{
		return std::sqrt(LengthSqr());
	}

	// Normalize a 2-D vector
	// NOTE: The result is unpredictable when all elements of vec are at or near zero
	inline void Normalize()
	{
		const float lenInv = 1.0f / Length();
		*this *= lenInv;
	}

	// Get normalized 2-D vector
	// NOTE: The result is unpredictable when all elements of vec are at or near zero
	inline Vector2 Normalized() const
	{
		const float lenInv = 1.0f / Length();
		return *this * lenInv;
	}

	// Compute the absolute value of a 2-D vector per element
	inline Vector2 AbsPerElem() const
	{
		return Vector2(std::abs(x), std::abs(y));
	}

	// Maximum of two 2-D vectors per element
	static inline Vector2 MaxPerElem(const Vector2& vec0, const Vector2& vec1)
	{
		return Vector2(
			(vec0.x > vec1.x) ? vec0.x : vec1.x,
			(vec0.y > vec1.y) ? vec0.y : vec1.y
		);
	}

	// Minimum of two 3-D vectors per element
	static inline Vector2 MinPerElem(const Vector2& vec0, const Vector2& vec1)
	{
		return Vector2(
			(vec0.x < vec1.x) ? vec0.x : vec1.x,
			(vec0.y < vec1.y) ? vec0.y : vec1.y
		);
	}

	// Construct axis 2-D vector
	static inline Vector2 AxisX()		{ return Vector2(1.0f, 0.0f); }
	static inline Vector2 AxisY()		{ return Vector2(0.0f, 1.0f); }

private:
	float x;
	float y;
};

// Multiply a 2-D vector by a scalar
inline Vector2 operator *(float scalar, const Vector2& vec)
{
	return vec * scalar;
}
