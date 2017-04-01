#pragma once

#include "Core/assert.h"

#include <stdint.h>

namespace Math
{

int RandInt(int rangeMin, int rangeMax);
float RandFloat(float rangeMin, float rangeMax);

inline float DegToRad(float deg)
{
	return deg * 0.0174532925f;			// deg * (PI / 180.0f)
}

inline float RadToDeg(float rad)
{
	return rad * 57.2957795130f;		// rad * (180.0f / PI)
}

inline int Round32(float x)
{
	return (int)std::roundf(x);
}

inline int RoundUp(float x, int roundFactor)
{
	ASSERT(roundFactor != 0);
	x -= 0.01f;
	return (int)std::ceil(x / roundFactor) * roundFactor;
}

inline float FastInvSqrt(float x)
{
	const float halfX = x * 0.5f;
	uint32_t tmp = *(uint32_t*)&x;		// get bits for floating value
	tmp = 0x5f3759df - (tmp >> 1);		// initial guess for Newton's method
	float y = *(float*)&tmp;			// convert bits back to float
	// Newton steps (repeating increases accuracy)
	y = y * (1.5f - halfX * y * y);		// 1st iteration
	return y;
}

// Returns linear interpolation of 'x' and 'y' with ratio 't'
// NOTE: Does not clamp 't' between 0 and 1
template<typename T>
inline T Lerp(T x, T y, float t)
{
	return x + (y - x) * t;
}

// Returns Hermite interpolation of 'x' and 'y' with ratio 't'
// NOTE: Does not clamp 't' between 0 and 1
template<typename T>
inline T HermiteInterpolation(T x, T y, float t)
{
	return Lerp(x, y, t * t * (3.0f - 2.0f * t));
}

inline bool IsPowerOfTwo(uint32_t x)
{
	return (x & (x - 1)) == 0;
}

// Efficient computation of integer powers. Computes a^e (mod UINT32_MAX)
inline uint32_t IntPow(uint32_t a, uint32_t e)
{
	uint32_t ret;
	for(ret = 1; e; e >>= 1)
	{
		if(e & 1)
		{
			ret *= a;
		}
		a *= a;
	}
	return ret;
}

// Integer select functions to avoid branches, which makes performance much better (especially for consoles)
// Returns 'x' if cond >= 0 and 'y' otherwise
inline int IntSel(int condition, int x, int y)
{
	// Set mask to 0xffffffff if condition < 0 and 0x00000000 otherwise (arithmetic shift right, splat out the sign bit)
	const int mask = condition >> 31;
	return x + ((y - x) & mask);
}

// Clamp 'x' to the specified minimum and maximum range
inline float Clamp(float x, float min, float max)
{
	ASSERT(!isnan(x));
#if defined(DESIRE_USE_SSE)
	_mm_store_ss(&x, _mm_max_ss(_mm_min_ss(_mm_load_ss(&x), _mm_load_ss(&max)), _mm_load_ss(&min)));
	return x;
#else
	return std::fmaxf(std::fminf(x, max), min);
#endif
}

// Clamp 'x' to the [0, 1] range
inline float Clamp01(float x)
{
	return Clamp(x, 0.0f, 1.0f);
}

}	// namespace Math
