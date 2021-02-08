#include "Engine/stdafx.h"
#include "Engine/Core/ConfigValue/ConfigValueInt.h"

ConfigValueInt::ConfigValueInt(const char* pName, const char* pDescription, int32_t initValue, int32_t minValue, int32_t maxValue)
	: IConfigValue(pName, pDescription)
	, m_value(initValue)
	, m_minValue(minValue)
	, m_maxValue(maxValue)
{
}

ConfigValueInt& ConfigValueInt::operator =(int32_t newValue)
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
