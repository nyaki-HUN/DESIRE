#pragma once

#include "Engine/Core/ConfigValue/IConfigValue.h"

class ConfigValueInt : public IConfigValue
{
public:
	ConfigValueInt(const char* name, const char* description, int initValue, int minValue = INT32_MIN, int maxValue = INT32_MAX);

	ConfigValueInt& operator =(int newValue);

	inline operator int() const
	{
		return value;
	}

	const int minValue;
	const int maxValue;

private:
	int value;
};
