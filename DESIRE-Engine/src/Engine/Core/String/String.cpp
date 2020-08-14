#include "Engine/stdafx.h"
#include "Engine/Core/String/String.h"

const String String::kEmptyString = "";

String::String(const char* pStr, size_t numChars)
	: pData(const_cast<char*>(pStr))
	, size(numChars)
{
	ASSERT(pData != nullptr);
	ASSERT(pData[size] == '\0');
}

size_t String::Find(const String& search, size_t pos) const
{
	if(search.size == 0 || pos >= size)
	{
		return kInvalidPos;
	}

	const char* pFoundCh = strstr(pData + pos, search.pData);
	return (pFoundCh != nullptr) ? pFoundCh - pData : kInvalidPos;
}

size_t String::Find(char search, size_t pos) const
{
	if(pos >= size || search == '\0')
	{
		return kInvalidPos;
	}

	const char* pFoundCh = strchr(pData + pos, search);
	return (pFoundCh != nullptr) ? pFoundCh - pData : kInvalidPos;
}

size_t String::FindLast(const String& search) const
{
	const char* s = pData + size - search.size;
	while(s >= pData)
	{
		if(memcmp(s, search.pData, search.size) == 0)
		{
			return static_cast<size_t>(s - pData);
		}

		s--;
	}

	return kInvalidPos;
}

size_t String::FindLast(char search) const
{
	const char* s = pData + size - 1;
	while(s >= pData)
	{
		if(*s == search)
		{
			return static_cast<size_t>(s - pData);
		}

		s--;
	}

	return kInvalidPos;
}

const char* String::Str() const
{
	return pData;
}

size_t String::Length() const
{
	return size;
}

size_t String::LengthUTF8() const
{
	size_t len = 0;
	const char* pCh = pData;
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
	return (size == 0);
}

int32_t String::AsInt32() const
{
	return std::strtol(pData, nullptr, 0);
}

int64_t String::AsInt64() const
{
	return std::strtoll(pData, nullptr, 0);
}

uint32_t String::AsUint32() const
{
	return std::strtoul(pData, nullptr, 0);
}

uint64_t String::AsUint64() const
{
	return std::strtoull(pData, nullptr, 0);
}

float String::AsFloat() const
{
	return std::strtof(pData, nullptr);
}

double String::AsDouble() const
{
	return std::strtod(pData, nullptr);
}

int String::Compare(const String& string) const
{
	const int rv = memcmp(pData, string.pData, std::min(size, string.size));
	return (rv != 0) ? rv : static_cast<int>(string.size) - static_cast<int>(size);
}

bool String::Equals(const String& string) const
{
	return (size == string.size) ? (memcmp(pData, string.pData, size) == 0) : false;
}

bool String::StartsWith(const String& prefix) const
{
	if(size >= prefix.size && prefix.size != 0)
	{
		return (memcmp(pData, prefix.pData, prefix.size) == 0);
	}

	return false;
}

bool String::StartsWith(char prefix) const
{
	return (pData[0] == prefix);
}

bool String::EndsWith(const String& suffix) const
{
	if(size >= suffix.size && suffix.size != 0)
	{
		return (memcmp(&pData[size - suffix.size], suffix.pData, suffix.size) == 0);
	}

	return false;
}

bool String::EndsWith(char suffix) const
{
	ASSERT(suffix != '\0');
	return (size < 1) ? false : (pData[size - 1] == suffix);
}

String String::SubString(size_t pos) const
{
	if(pos < size)
	{
		return String(&pData[pos], size - pos);
	}

	return String();
}
