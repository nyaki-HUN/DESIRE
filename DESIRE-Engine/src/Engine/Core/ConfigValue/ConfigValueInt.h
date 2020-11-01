#pragma once

#include "Engine/Core/ConfigValue/IConfigValue.h"

class ConfigValueInt : public IConfigValue
{
public:
	ConfigValueInt(const char* pName, const char* pDescription, int initValue, int minValue = INT32_MIN, int maxValue = INT32_MAX);

	ConfigValueInt& operator =(int newValue);

	inline operator int() const
	{
		return m_value;
	}

	const int m_minValue;
	const int m_maxValue;

private:
	int m_value;
};
