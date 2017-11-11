#pragma once

// --------------------------------------------------------------------------------------------------------------------
//	A two-dimensional position implementation
// --------------------------------------------------------------------------------------------------------------------

template<typename T>
class Point
{
public:
	Point<T>()
	{

	}

	Point<T>(T x_, T y_)
	{
		x = x_;
		y = y_;
	}

	Point<T>(const Point<T>& p)
	{
		x = p.x;
		y = p.y;
	}

	inline void Set(T x_, T y_)
	{
		x = x_;
		y = y_;
	}

	inline bool operator ==(const Point<T>& p) const
	{
		return (x == p.x && y == p.y);
	}

	inline bool operator !=(const Point<T>& p) const
	{
		return (x != p.x || y != p.y);
	}

	inline void operator +=(const Point<T>& p)
	{
		x += p.x;
		y += p.y;
	}

	inline void operator -=(const Point<T>& p)
	{
		x -= p.x;
		y -= p.y;
	}

	T x, y;
};

template<typename T>
inline Point<T> operator +(const Point<T>& p1, const Point<T>& p2)
{
	return Point<T>(p1.x + p2.x, p1.y + p2.y);
}

template<typename T>
inline Point<T> operator -(const Point<T>& p1, const Point<T>& p2)
{
	return Point<T>(p1.x - p2.x, p1.y - p2.y);
}
