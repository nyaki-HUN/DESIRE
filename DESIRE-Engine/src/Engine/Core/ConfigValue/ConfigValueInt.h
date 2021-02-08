#pragma once

#include "Engine/Core/ConfigValue/IConfigValue.h"

class ConfigValueInt : public IConfigValue
{
public:
	ConfigValueInt(const char* pName, const char* pDescription, int32_t initValue, int32_t minValue = INT32_MIN, int32_t maxValue = INT32_MAX);

	ConfigValueInt& operator =(int32_t newValue);

	operator int() const	{ return m_value; }

	const int32_t m_minValue;
	const int32_t m_maxValue;

private:
	int32_t m_value;
};
