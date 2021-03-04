#include "Engine/stdafx.h"
#include "Engine/Core/Math/math.h"

namespace Math
{

int32_t Round32(float x)
{
	return static_cast<int32_t>(std::round(x));
}

int32_t RoundUp(float x, int32_t roundFactor)
{
	ASSERT(roundFactor != 0);
	x -= 0.01f;
	return static_cast<int32_t>(std::ceil(x / roundFactor) * roundFactor);
}

float Sqrt(float x)
{
#if DESIRE_USE_SSE
	return _mm_cvtss_f32(_mm_sqrt_ss(_mm_set_ss(x)));
#else
	return std::sqrt(x);
#endif
}

bool IsPowerOfTwo(size_t x)
{
	return (x & (x - 1)) == 0;
}

uint32_t SafeSizeToUint32(size_t value)
{
	ASSERT(value <= UINT32_MAX);
	return static_cast<uint32_t>(value);
}

uint32_t IntPow(uint32_t a, uint32_t e)
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

int32_t IntSel(int32_t condition, int32_t x, int32_t y)
{
	// Set mask to 0xffffffff if condition < 0 and 0x00000000 otherwise (arithmetic shift right, splat out the sign bit)
	const int32_t mask = condition >> 31;
	return x + ((y - x) & mask);
}

float FloatsAreEqual(float a, float b, float tolerance)
{
	return std::abs(a - b) < tolerance;
}

float FloatIsZero(float x, float tolerance)
{
	return std::abs(x) < tolerance;
}

}	// namespace Math
