#include "Engine/stdafx.h"
#include "Engine/Core/math/math.h"

#include <random>

namespace Math
{

int Round32(float x)
{
	return (int)std::round(x);
}

int RoundUp(float x, int roundFactor)
{
	ASSERT(roundFactor != 0);
	x -= 0.01f;
	return (int)std::ceil(x / roundFactor) * roundFactor;
}

float FastInvSqrt(float x)
{
	const float halfX = x * 0.5f;
	uint32_t tmp = *(uint32_t*)&x;		// get bits for floating value
	tmp = 0x5f3759df - (tmp >> 1);		// initial guess for Newton's method
	float y = *(float*)&tmp;			// convert bits back to float
	// Newton steps (repeating increases accuracy)
	y = y * (1.5f - halfX * y * y);		// 1st iteration
	return y;
}

bool IsPowerOfTwo(uint32_t x)
{
	return (x & (x - 1)) == 0;
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

int IntSel(int condition, int x, int y)
{
	// Set mask to 0xffffffff if condition < 0 and 0x00000000 otherwise (arithmetic shift right, splat out the sign bit)
	const int mask = condition >> 31;
	return x + ((y - x) & mask);
}

float Clamp(float x, float min, float max)
{
	ASSERT(!isnan(x));
#if defined(DESIRE_USE_SSE)
	_mm_store_ss(&x, _mm_max_ss(_mm_min_ss(_mm_load_ss(&x), _mm_load_ss(&max)), _mm_load_ss(&min)));
	return x;
#else
	return std::max(std::min(x, max), min);
#endif
}

float Clamp01(float x)
{
	return Clamp(x, 0.0f, 1.0f);
}

}	// namespace Math
