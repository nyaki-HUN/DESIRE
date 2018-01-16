#include "Engine/stdafx.h"
#include "Engine/Core/ConfigValue/ConfigValueInt.h"

ConfigValueInt::ConfigValueInt(const char *name, const char *description, int initValue, int minValue, int maxValue)
	: IConfigValue(name, description)
	, value(initValue)
	, minValue(minValue)
	, maxValue(maxValue)
{

}

ConfigValueInt& ConfigValueInt::operator=(int newValue)
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
