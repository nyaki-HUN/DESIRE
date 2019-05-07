#include "Engine/stdafx.h"
#include "Engine/Core/math/Rand.h"
#include "Engine/Core/math/math.h"

Rand Rand::s_globalRand;

Rand::Rand()
{
	std::random_device randomDevice;
	randomEngine.seed(randomDevice());
}

int Rand::GetInt(int rangeMin, int rangeMax)
{
	std::uniform_int_distribution<int> distribution(rangeMin, rangeMax);
	return distribution(randomEngine);
}

uint32_t Rand::GetUint(uint32_t rangeMin, uint32_t rangeMax)
{
	std::uniform_int_distribution<uint32_t> distribution(rangeMin, rangeMax);
	return distribution(randomEngine);
}

float Rand::GetFloat(float rangeMin, float rangeMax)
{
	std::uniform_real_distribution<float> distribution(rangeMin, std::nextafter(rangeMax, FLT_MAX));
	return distribution(randomEngine);
}

double Rand::GetDouble(double rangeMin, double rangeMax)
{
	std::uniform_real_distribution<double> distribution(rangeMin, std::nextafter(rangeMax, DBL_MAX));
	return distribution(randomEngine);
}

bool Rand::GetBool(double probabilityOfTrue)
{
	std::bernoulli_distribution uniformDist(probabilityOfTrue);
	return uniformDist(randomEngine);
}
