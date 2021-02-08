#pragma once

#include "Engine/Core/ConfigValue/IConfigValue.h"

class ConfigValueFloat : public IConfigValue
{
public:
	ConfigValueFloat(const char* pName, const char* pDescription, float initValue, float minValue = -FLT_MAX, float maxValue = FLT_MAX);

	ConfigValueFloat& operator =(float newValue);

	operator float() const	{ return m_value; }

	const float m_minValue;
	const float m_maxValue;

private:
	float m_value;
};
