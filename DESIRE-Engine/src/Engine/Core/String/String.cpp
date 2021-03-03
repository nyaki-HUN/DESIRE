#include "Engine/stdafx.h"
#include "Engine/Core/String/String.h"

const String String::kEmptyString = "";

String::String(const char* pStr, size_t numChars)
	: m_pData(const_cast<char*>(pStr))
	, m_size(numChars)
{
	ASSERT(m_pData != nullptr);
	ASSERT(m_pData[m_size] == '\0');
}

size_t String::Find(const String& search, size_t pos) const
{
	if(search.m_size == 0 || pos >= m_size)
	{
		return kInvalidPos;
	}

	const char* pFoundCh = strstr(m_pData + pos, search.m_pData);
	return pFoundCh ? pFoundCh - m_pData : kInvalidPos;
}

size_t String::Find(char search, size_t pos) const
{
	if(pos >= m_size || search == '\0')
	{
		return kInvalidPos;
	}

	const char* pFoundCh = strchr(m_pData + pos, search);
	return pFoundCh ? pFoundCh - m_pData : kInvalidPos;
}

size_t String::FindLast(const String& search) const
{
	const char* s = m_pData + m_size - search.m_size;
	while(s >= m_pData)
	{
		if(memcmp(s, search.m_pData, search.m_size) == 0)
		{
			return static_cast<size_t>(s - m_pData);
		}

		s--;
	}

	return kInvalidPos;
}

size_t String::FindLast(char search) const
{
	const char* s = m_pData + m_size - 1;
	while(s >= m_pData)
	{
		if(*s == search)
		{
			return static_cast<size_t>(s - m_pData);
		}

		s--;
	}

	return kInvalidPos;
}

const char* String::Str() const
{
	return m_pData;
}

size_t String::Length() const
{
	return m_size;
}

size_t String::LengthUTF8() const
{
	size_t len = 0;
	const char* pCh = m_pData;
	while(*pCh != '\0')
	{
		if(*pCh > 0)
		{
			pCh++;
		}
		else
		{
			// Select amongst multi-byte starters
			switch(*pCh & 0xF0)
			{
				case 0xE0:	pCh += 3; break;
				case 0xF0:	pCh += 4; break;
				default:	pCh += 2; break;
			}
		}
		len++;
	}

	return len;
}

bool String::IsEmpty() const
{
	return (m_size == 0);
}

int32_t String::AsInt32() const
{
	return std::strtol(m_pData, nullptr, 0);
}

int64_t String::AsInt64() const
{
	return std::strtoll(m_pData, nullptr, 0);
}

uint32_t String::AsUint32() const
{
	return std::strtoul(m_pData, nullptr, 0);
}

uint64_t String::AsUint64() const
{
	return std::strtoull(m_pData, nullptr, 0);
}

float String::AsFloat() const
{
	return std::strtof(m_pData, nullptr);
}

double String::AsDouble() const
{
	return std::strtod(m_pData, nullptr);
}

int32_t String::Compare(const String& string) const
{
	const int32_t rv = memcmp(m_pData, string.m_pData, std::min(m_size, string.m_size));
	return (rv != 0) ? rv : static_cast<int>(string.m_size) - static_cast<int>(m_size);
}

bool String::Equals(const String& string) const
{
	return (m_size == string.m_size && memcmp(m_pData, string.m_pData, m_size) == 0);
}

bool String::StartsWith(const String& prefix) const
{
	if(m_size >= prefix.m_size && prefix.m_size != 0)
	{
		return (memcmp(m_pData, prefix.m_pData, prefix.m_size) == 0);
	}

	return false;
}

bool String::StartsWith(char prefix) const
{
	return (m_pData[0] == prefix);
}

bool String::EndsWith(const String& suffix) const
{
	if(m_size >= suffix.m_size && suffix.m_size != 0)
	{
		return (memcmp(&m_pData[m_size - suffix.m_size], suffix.m_pData, suffix.m_size) == 0);
	}

	return false;
}

bool String::EndsWith(char suffix) const
{
	ASSERT(suffix != '\0');
	return (m_size < 1) ? false : (m_pData[m_size - 1] == suffix);
}

String String::SubString(size_t pos) const
{
	if(pos < m_size)
	{
		return String(&m_pData[pos], m_size - pos);
	}

	return String();
}
