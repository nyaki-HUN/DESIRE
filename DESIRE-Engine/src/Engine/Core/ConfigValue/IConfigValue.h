#pragma once

class IConfigValue
{
public:
	IConfigValue(const char* name, const char* description);
	~IConfigValue();

	// Tries to find a config value
	static IConfigValue* FindConfigValue(const char* name);

	IConfigValue* next;
	const char* const name;
	const char* const description;

	static IConfigValue* s_listHead;

private:
	static IConfigValue* s_listTail;
};
