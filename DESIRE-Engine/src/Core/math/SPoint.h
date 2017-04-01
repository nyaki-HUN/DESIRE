#pragma once

// --------------------------------------------------------------------------------------------------------------------
//	A two-dimensional position implementation
// --------------------------------------------------------------------------------------------------------------------

template<typename T>
struct SPoint
{
	SPoint<T>()
	{

	}

	SPoint<T>(T x_, T y_)
	{
		x = x_;
		y = y_;
	}

	SPoint<T>(const SPoint<T>& p)
	{
		x = p.x;
		y = p.y;
	}

	inline void Set(T x_, T y_)
	{
		x = x_;
		y = y_;
	}

	inline bool operator ==(const SPoint<T>& p) const
	{
		return (x == p.x && y == p.y);
	}

	inline bool operator !=(const SPoint<T>& p) const
	{
		return (x != p.x || y != p.y);
	}

	inline void operator +=(const SPoint<T>& p)
	{
		x += p.x;
		y += p.y;
	}

	inline void operator -=(const SPoint<T>& p)
	{
		x -= p.x;
		y -= p.y;
	}

	T x, y;
};

template<typename T>
inline SPoint<T> operator +(const SPoint<T>& p1, const SPoint<T>& p2)
{
	return SPoint<T>(p1.x + p2.x, p1.y + p2.y);
}

template<typename T>
inline SPoint<T> operator -(const SPoint<T>& p1, const SPoint<T>& p2)
{
	return SPoint<T>(p1.x - p2.x, p1.y - p2.y);
}
