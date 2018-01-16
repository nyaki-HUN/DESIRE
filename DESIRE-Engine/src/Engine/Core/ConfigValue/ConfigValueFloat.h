#pragma once

#include "Engine/Core/ConfigValue/IConfigValue.h"

#include <float.h>		// for FLT_MAX

class ConfigValueFloat : public IConfigValue
{
public:
	ConfigValueFloat(const char *name, const char *description, float initValue, float minValue = -FLT_MAX, float maxValue = FLT_MAX);

	ConfigValueFloat& operator =(float newValue);

	inline operator float() const
	{
		return value;
	}

	const float minValue;
	const float maxValue;

private:
	float value;
};
