#pragma once

class Vector2
{
public:
	inline Vector2()													{}
	inline Vector2(const Vector2& vec)		: x(vec.x), y(vec.y)		{}
	inline Vector2(float x, float y)		: x(x) , y(y)				{}
	explicit inline Vector2(float scalar)	: Vector2(scalar, scalar)	{}

	inline void SetX(float _x)								{ x = _x; }
	inline void SetY(float _y)								{ y = _y; }

	inline float GetX() const								{ return x; }
	inline float GetY() const								{ return y; }

	inline Vector2 SetX(float _x) const						{ return Vector2(_x, y); }
	inline Vector2 SetY(float _y) const						{ return Vector2(x, _y); }

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

	inline bool operator >(const Vector2& vec) const		{ return (x > vec.x && y > vec.y); }
	inline bool operator <(const Vector2& vec) const		{ return (x < vec.x && y < vec.y); }
	inline bool operator >=(const Vector2& vec) const		{ return (x >= vec.x && y >= vec.y); }
	inline bool operator <=(const Vector2& vec) const		{ return (x <= vec.x && y <= vec.y); }

	inline float Dot(const Vector2& vec) const				{ return x * vec.x + y * vec.y; }

	inline float LengthSqr() const							{ return Dot(*this); }
	inline float Length() const								{ return std::sqrt(LengthSqr()); }

	inline void Normalize()
	{
		const float lenInv = 1.0f / Length();
		*this *= lenInv;
	}

	inline Vector2 Normalized() const
	{
		const float lenInv = 1.0f / Length();
		return *this * lenInv;
	}

	inline Vector2 AbsPerElem() const						{ return Vector2(std::abs(x), std::abs(y)); }

	inline float GetMaxElem() const							{ return (x > y) ? x : y; }
	inline float GetMinElem() const							{ return (x < y) ? x : y; }

	static inline Vector2 MaxPerElem(const Vector2& a, const Vector2& b)	{ return Vector2((a.x > b.x) ? a.x : b.x, (a.y > b.y) ? a.y : b.y); }
	static inline Vector2 MinPerElem(const Vector2& a, const Vector2& b)	{ return Vector2((a.x < b.x) ? a.x : b.x, (a.y < b.y) ? a.y : b.y); }

	static inline Vector2 Zero()		{ return Vector2(0.0f); }
	static inline Vector2 One()			{ return Vector2(1.0f); }
	static inline Vector2 AxisX()		{ return Vector2(1.0f, 0.0f); }
	static inline Vector2 AxisY()		{ return Vector2(0.0f, 1.0f); }

private:
	float x = 0.0f;
	float y = 0.0f;
};

inline Vector2 operator *(float scalar, const Vector2& vec)
{
	return vec * scalar;
}
