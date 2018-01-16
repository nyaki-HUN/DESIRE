#include "stdafx.h"
#include "Core/math/math.h"

#include <random>

namespace Math
{

thread_local std::mt19937 rndGenerator(std::random_device{}());

int RandInt(int rangeMin, int rangeMax)
{
	std::uniform_int_distribution<int> distribution(rangeMin, rangeMax);
	int num = distribution(rndGenerator);
	return num;
}

float RandFloat(float rangeMin, float rangeMax)
{
	std::uniform_real_distribution<float> distribution(rangeMin, std::nextafter(rangeMax, FLT_MAX));
	float num = distribution(rndGenerator);
	return num;
}

}	// namespace Math
