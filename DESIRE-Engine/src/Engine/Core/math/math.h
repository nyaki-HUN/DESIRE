#pragma once

#include <stdint.h>

namespace Math
{

constexpr float PI = 3.14159265358979323846f;
constexpr float TAU = (2.0f * PI);
constexpr float PI_2 = (PI / 2.0f);
constexpr float PI_4 = (PI / 4.0f);

int Round32(float x);
int RoundUp(float x, int roundFactor);

float FastInvSqrt(float x);

bool IsPowerOfTwo(uint32_t x);

// Efficient computation of integer powers. Computes a^e (mod UINT32_MAX)
uint32_t IntPow(uint32_t a, uint32_t e);

// Integer select functions to avoid branches, which makes performance much better (especially for consoles)
// Returns 'x' if cond >= 0 and 'y' otherwise
inline int IntSel(int condition, int x, int y);

// Clamp 'x' to the [min, max] range
float Clamp(float x, float min, float max);

// Clamp 'x' to the [0, 1] range
float Clamp01(float x);

template<typename T>
constexpr T DegToRad(T deg)
{
	return deg * 0.0174532925f;			// deg * (PI / 180.0f)
}

template<typename T>
constexpr T RadToDeg(T rad)
{
	return rad * 57.2957795130f;		// rad * (180.0f / PI)
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

}	// end of namespace Math
