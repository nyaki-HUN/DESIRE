#include "Engine/stdafx.h"
#include "Engine/Core/ConfigValue/ConfigValueFloat.h"

ConfigValueFloat::ConfigValueFloat(const char* name, const char* description, float initValue, float minValue, float maxValue)
	: IConfigValue(name, description)
	, value(initValue)
	, minValue(minValue)
	, maxValue(maxValue)
{

}

ConfigValueFloat& ConfigValueFloat::operator =(float newValue)
{
	if(newValue < minValue)
	{
		newValue = minValue;
	}
	else if(newValue > maxValue)
	{
		newValue = maxValue;
	}

	value = newValue;
	return *this;
}
