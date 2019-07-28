#include "Engine/stdafx.h"
#include "Engine/Core/ConfigValue/IConfigValue.h"

IConfigValue* IConfigValue::s_listHead = nullptr;
IConfigValue* IConfigValue::s_listTail = nullptr;

IConfigValue::IConfigValue(const char* name, const char* description)
	: next(nullptr)
	, name(name)
	, description(description)
{
	ASSERT(name != nullptr);

	if(s_listHead == nullptr)
	{
		s_listHead = this;
	}
	else
	{
		s_listTail->next = this;
	}

	s_listTail = this;
}

IConfigValue::~IConfigValue()
{
	if(this == s_listHead)
	{
		s_listHead = s_listHead->next;
		return;
	}

	IConfigValue* prevConfig = s_listHead;
	while(this != prevConfig->next)
	{
		prevConfig = prevConfig->next;
	}

	prevConfig->next = prevConfig->next->next;
}

IConfigValue* IConfigValue::FindConfigValue(const char* name)
{
	IConfigValue* config = s_listHead;
	while(config != nullptr && strcmp(config->name, name) != 0)
	{
		config = config->next;
	}

	return config;
}
