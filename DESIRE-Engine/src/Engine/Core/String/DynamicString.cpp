#include "Engine/stdafx.h"
#include "Engine/Core/String/DynamicString.h"

#include "Engine/Core/Memory/MemorySystem.h"

static char s_emptyData[] = { '\0' };

DynamicString::DynamicString()
{
	m_pData = s_emptyData;
}

DynamicString::DynamicString(const char* str, size_t size)
{
	Set(str, size);
}

DynamicString::DynamicString(DynamicString&& string)
{
	m_pData = string.m_pData;
	m_size = string.m_size;
	m_preallocatedSize = string.m_preallocatedSize;

	string.m_pData = s_emptyData;
	string.m_size = 0;
	string.m_preallocatedSize = 0;
}

DynamicString::~DynamicString()
{
	Reset();
}

DynamicString& DynamicString::operator =(DynamicString&& string)
{
	ASSERT(this != &string);	// It's not allowed to copy from ourself

	Reset();

	m_pData = string.m_pData;
	m_size = string.m_size;
	m_preallocatedSize = string.m_preallocatedSize;

	string.m_pData = s_emptyData;
	string.m_size = 0;
	string.m_preallocatedSize = 0;

	return *this;
}

DynamicString DynamicString::SubString(size_t pos, size_t numChars) const
{
	if(pos < m_size)
	{
		return DynamicString(&m_pData[pos], std::min(numChars, m_size - pos));
	}

	return DynamicString();
}

void DynamicString::Reset()
{
	if(m_pData != s_emptyData)
	{
		MemorySystem::Free(m_pData);
		m_pData = s_emptyData;
		m_size = 0;
		m_preallocatedSize = 0;
	}
}

bool DynamicString::Reserve(size_t numChars)
{
	if(m_preallocatedSize < numChars + 1)
	{
		m_preallocatedSize = numChars + 1;

		if(m_pData != s_emptyData)
		{
			m_pData = static_cast<char*>(MemorySystem::Realloc(m_pData, m_preallocatedSize * sizeof(char)));
		}
		else
		{
			m_pData = static_cast<char*>(MemorySystem::Alloc(m_preallocatedSize * sizeof(char)));
		}
	}

	return true;
}
