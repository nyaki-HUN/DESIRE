#include "Engine/stdafx.h"
#include "Engine/Core/ConfigValue/IConfigValue.h"

IConfigValue* IConfigValue::s_pListHead = nullptr;
IConfigValue* IConfigValue::s_pListTail = nullptr;

IConfigValue::IConfigValue(const char* pName, const char* pDescription)
	: m_pNext(nullptr)
	, m_pName(pName)
	, m_pDescription(pDescription)
{
	ASSERT(m_pName != nullptr);

	if(s_pListHead == nullptr)
	{
		s_pListHead = this;
	}
	else
	{
		s_pListTail->m_pNext = this;
	}

	s_pListTail = this;
}

IConfigValue::~IConfigValue()
{
	if(this == s_pListHead)
	{
		s_pListHead = s_pListHead->m_pNext;
		return;
	}

	IConfigValue* pPrevConfig = s_pListHead;
	while(this != pPrevConfig->m_pNext)
	{
		pPrevConfig = pPrevConfig->m_pNext;
	}

	pPrevConfig->m_pNext = pPrevConfig->m_pNext->m_pNext;
}

IConfigValue* IConfigValue::FindConfigValue(const char* pName)
{
	IConfigValue* pConfig = s_pListHead;
	while(pConfig && strcmp(pConfig->m_pName, pName) != 0)
	{
		pConfig = pConfig->m_pNext;
	}

	return pConfig;
}
