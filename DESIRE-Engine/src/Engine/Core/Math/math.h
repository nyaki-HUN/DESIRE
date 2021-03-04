#pragma once

namespace Math
{

constexpr float Pi = 3.14159265358979323846f;
constexpr float Tau = 2.0f * Pi;
constexpr float Pi_2 = Pi / 2.0f;
constexpr float Pi_4 = Pi / 4.0f;

int32_t Round32(float x);
int32_t RoundUp(float x, int32_t roundFactor);

float Sqrt(float x);

bool IsPowerOfTwo(size_t x);
uint32_t SafeSizeToUint32(size_t value);

// Efficient computation of integer powers. Computes a^e (mod UINT32_MAX)
uint32_t IntPow(uint32_t a, uint32_t e);

// Integer select functions to avoid branches, which makes performance much better (especially for consoles)
// Returns 'x' if cond >= 0 and 'y' otherwise
int32_t IntSel(int32_t condition, int32_t x, int32_t y);

float FloatsAreEqual(float a, float b, float tolerance = FLT_EPSILON);
float FloatIsZero(float x, float tolerance = FLT_EPSILON);

template<typename T>
constexpr T DegToRad(T deg)
{
	return deg * 0.0174532925f;			// deg * (Pi / 180.0f)
}

template<typename T>
constexpr T RadToDeg(T rad)
{
	return rad * 57.2957795130f;		// rad * (180.0f / Pi)
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
