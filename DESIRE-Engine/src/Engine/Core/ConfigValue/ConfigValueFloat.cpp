#include "Engine/stdafx.h"
#include "Engine/Core/ConfigValue/ConfigValueFloat.h"

ConfigValueFloat::ConfigValueFloat(const char* pName, const char* pDescription, float initValue, float minValue, float maxValue)
	: IConfigValue(pName, pDescription)
	, m_value(initValue)
	, m_minValue(minValue)
	, m_maxValue(maxValue)
{
}

ConfigValueFloat& ConfigValueFloat::operator =(float newValue)
{
	if(newValue < m_minValue)
	{
		newValue = m_minValue;
	}
	else if(newValue > m_maxValue)
	{
		newValue = m_maxValue;
	}

	m_value = newValue;
	return *this;
}
