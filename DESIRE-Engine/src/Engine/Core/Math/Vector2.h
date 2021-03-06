#pragma once

class Vector2
{
public:
	inline Vector2() = default;
	inline Vector2(float x, float y) : m_x(x), m_y(y)		{}

	inline void SetX(float x)								{ m_x = x; }
	inline void SetY(float y)								{ m_y = y; }

	inline float GetX() const								{ return m_x; }
	inline float GetY() const								{ return m_y; }

	inline Vector2 SetX(float x) const						{ return Vector2(x, m_y); }
	inline Vector2 SetY(float y) const						{ return Vector2(m_x, y); }

	inline Vector2& operator =(const Vector2& vec)
	{
		m_x = vec.m_x;
		m_y = vec.m_y;
		return *this;
	}

	inline Vector2 operator -() const						{ return Vector2(-m_x, -m_y); }
	inline Vector2 operator +(const Vector2& vec) const		{ return Vector2(m_x + vec.m_x, m_y + vec.m_y); }
	inline Vector2 operator -(const Vector2& vec) const		{ return Vector2(m_x - vec.m_x, m_y - vec.m_y); }
	inline Vector2 operator *(const Vector2& vec) const		{ return Vector2(m_x * vec.m_x, m_y * vec.m_y); }
	inline Vector2 operator *(float scalar) const			{ return Vector2(m_x * scalar, m_y * scalar); }
	inline Vector2 operator /(const Vector2& vec) const		{ return Vector2(m_x / vec.m_x, m_y / vec.m_y); }
	inline Vector2 operator /(float scalar) const			{ return Vector2(m_x / scalar, m_y / scalar); }

	inline Vector2& operator +=(const Vector2& vec)			{ *this = *this + vec;		return *this; }
	inline Vector2& operator -=(const Vector2& vec)			{ *this = *this - vec;		return *this; }
	inline Vector2& operator *=(const Vector2& vec)			{ *this = *this * vec;		return *this; }
	inline Vector2& operator *=(float scalar)				{ *this = *this * scalar;	return *this; }
	inline Vector2& operator /=(const Vector2& vec)			{ *this = *this / vec;		return *this; }
	inline Vector2& operator /=(float scalar)				{ *this = *this / scalar;	return *this; }

	inline bool operator >(const Vector2& vec) const		{ return (m_x > vec.m_x && m_y > vec.m_y); }
	inline bool operator <(const Vector2& vec) const		{ return (m_x < vec.m_x && m_y < vec.m_y); }
	inline bool operator >=(const Vector2& vec) const		{ return (m_x >= vec.m_x && m_y >= vec.m_y); }
	inline bool operator <=(const Vector2& vec) const		{ return (m_x <= vec.m_x && m_y <= vec.m_y); }

	inline float Dot(const Vector2& vec) const				{ return m_x * vec.m_x + m_y * vec.m_y; }

	inline float LengthSqr() const							{ return Dot(*this); }
	inline float Length() const								{ return std::sqrt(LengthSqr()); }

	inline void Normalize()									{ *this = Normalized(); }
	inline Vector2 Normalized() const						{ const float lenInv = 1.0f / Length(); return *this * lenInv; }

	inline Vector2 AbsPerElem() const						{ return Vector2(std::abs(m_x), std::abs(m_y)); }

	inline float GetMaxElem() const							{ return (m_x > m_y) ? m_x : m_y; }
	inline float GetMinElem() const							{ return (m_x < m_y) ? m_x : m_y; }

	static inline Vector2 MaxPerElem(const Vector2& a, const Vector2& b)	{ return Vector2((a.m_x > b.m_x) ? a.m_x : b.m_x, (a.m_y > b.m_y) ? a.m_y : b.m_y); }
	static inline Vector2 MinPerElem(const Vector2& a, const Vector2& b)	{ return Vector2((a.m_x < b.m_x) ? a.m_x : b.m_x, (a.m_y < b.m_y) ? a.m_y : b.m_y); }

	static inline Vector2 Zero()		{ return Vector2(0.0f, 0.0f); }
	static inline Vector2 One()			{ return Vector2(1.0f, 1.0f); }
	static inline Vector2 AxisX()		{ return Vector2(1.0f, 0.0f); }
	static inline Vector2 AxisY()		{ return Vector2(0.0f, 1.0f); }

private:
	float m_x = 0.0f;
	float m_y = 0.0f;
};

inline Vector2 operator *(float scalar, const Vector2& vec)
{
	return vec * scalar;
}
