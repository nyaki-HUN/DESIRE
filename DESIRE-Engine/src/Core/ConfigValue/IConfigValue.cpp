#include "stdafx.h"
#include "Core/ConfigValue/IConfigValue.h"

IConfigValue *IConfigValue::listHead = nullptr;
IConfigValue *IConfigValue::listTail = nullptr;

IConfigValue::IConfigValue(const char *name, const char *description)
	: name(name)
	, description(description)
{
	ASSERT(name != nullptr);

	if(listHead == nullptr)
	{
		listHead = this;
	}
	else
	{
		listTail->next = this;
	}

	listTail = this;
}

IConfigValue::~IConfigValue()
{
	if(this == listHead)
	{
		listHead = listHead->next;
		return;
	}

	IConfigValue *prevConfig = listHead;
	while(this != prevConfig->next)
	{
		prevConfig = prevConfig->next;
	}

	prevConfig->next = prevConfig->next->next;
}

IConfigValue* IConfigValue::FindConfigValue(const char *name)
{
	IConfigValue *config = listHead;
	while(config != nullptr && strcmp(config->name, name) != 0)
	{
		config = config->next;
	}

	return config;
}
