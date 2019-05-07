#pragma once

#include <random>

class Rand
{
public:
	Rand();

	// Integer numbers
	int GetInt(int rangeMin = INT32_MIN, int rangeMax = INT32_MAX);
	uint32_t GetUint(uint32_t rangeMin = 0, uint32_t rangeMax = UINT32_MAX);

	// Real numbers
	float GetFloat(float rangeMin, float rangeMax);
	double GetDouble(double rangeMin, double rangeMax);

	// Bool
	bool GetBool(double probabilityOfTrue = 0.5);

	static Rand s_globalRand;

private:
	std::default_random_engine randomEngine;
};
