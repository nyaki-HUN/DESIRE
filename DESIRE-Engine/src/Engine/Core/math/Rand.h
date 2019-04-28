#pragma once

#include <random>

class Rand
{
public:
	Rand();

	// Integer numbers
	int GetInt(int rangeMin, int rangeMax);
	uint32_t GetUint(uint32_t rangeMin, uint32_t rangeMax);

	// Real numbers
	float GetFloat(float rangeMin, float rangeMax);
	double GetDouble(double rangeMin, double rangeMax);

	// Bool
	bool GetBool(double probabilityOfTrue = 0.5);

	static Rand globalRand;

private:
	std::default_random_engine randomEngine;
};
