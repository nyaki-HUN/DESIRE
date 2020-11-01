#pragma once

class IConfigValue
{
public:
	IConfigValue(const char* pName, const char* pDescription);
	~IConfigValue();

	// Tries to find a config value
	static IConfigValue* FindConfigValue(const char* pName);

	IConfigValue* m_pNext;
	const char* const m_pName;
	const char* const m_pDescription;

	static IConfigValue* s_pListHead;

private:
	static IConfigValue* s_pListTail;
};
