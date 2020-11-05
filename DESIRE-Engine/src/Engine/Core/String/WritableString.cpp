#include "Engine/stdafx.h"
#include "Engine/Core/String/WritableString.h"

void WritableString::Clear()
{
	m_size = 0;
	m_pData[m_size] = '\0';
}

void WritableString::Insert(size_t pos, const char* pStr, size_t numChars)
{
	ASSERT(pStr != nullptr);

	if(pos > m_size)
	{
		return;
	}

	if(!Reserve(m_size + numChars))
	{
		ASSERT(false);
		return;
	}

	std::memmove(m_pData + pos + numChars, m_pData + pos, m_size - pos + 1);
	std::memcpy(m_pData + pos, pStr, numChars);
	m_size += numChars;
}

void WritableString::RemoveFrom(size_t pos, size_t numChars)
{
	if(pos >= m_size)
	{
		return;
	}

	numChars = std::min(numChars, m_size - pos);

	std::memmove(m_pData + pos, m_pData + pos + numChars, m_size - pos - numChars + 1);
	m_size -= numChars;
}

void WritableString::RemoveFromEnd(size_t numChars)
{
	m_size -= (numChars <= m_size) ? numChars : m_size;
	m_pData[m_size] = '\0';
}

void WritableString::TruncateAt(size_t pos)
{
	if(pos >= m_size)
	{
		return;
	}

	m_pData[pos] = '\0';
	m_size = pos;
}

void WritableString::Replace(const String& search, const String& replaceTo)
{
	const size_t foundPos = Find(search);
	if(foundPos == kInvalidPos)
	{
		return;
	}

	const int64_t numCharsToMove = replaceTo.Length() - search.Length();
	if(numCharsToMove > 0)
	{
		if(!Reserve(m_size + numCharsToMove))
		{
			ASSERT(false);
			return;
		}
	}

	Replace_Internal(foundPos, search.Length(), replaceTo);
}

void WritableString::ReplaceAll(const String& search, const String& replaceTo)
{
	size_t foundPos = Find(search);
	if(foundPos == kInvalidPos)
	{
		return;
	}

	const int64_t numCharsToMove = replaceTo.Length() - search.Length();
	if(numCharsToMove > 0)
	{
		size_t count = 0;
		size_t pos = foundPos;
		do
		{
			count++;
			pos = Find(search, pos + search.Length());
		} while(pos != kInvalidPos);

		if(!Reserve(m_size + count * numCharsToMove))
		{
			ASSERT(false);
			return;
		}
	}

	do
	{
		Replace_Internal(foundPos, search.Length(), replaceTo);
		foundPos = Find(search, foundPos + replaceTo.Length());
	} while(foundPos != kInvalidPos);
}

void WritableString::ReplaceAllChar(char search, char replaceTo)
{
	ASSERT(search != replaceTo);

	size_t foundPos = Find(search);
	if(foundPos == kInvalidPos)
	{
		return;
	}

	// When replacing to the null-character the string has to be truncated
	if(replaceTo == '\0')
	{
		TruncateAt(foundPos);
		return;
	}

	do
	{
		m_pData[foundPos] = replaceTo;
		foundPos = Find(search, foundPos + 1);
	} while(foundPos != kInvalidPos);
}

void WritableString::Append(const String& string)
{
	const size_t numChars = string.Length();
	if(!Reserve(m_size + numChars))
	{
		ASSERT(false);
		return;
	}

	std::memcpy(m_pData + m_size, string.Str(), numChars);
	m_size += numChars;
	m_pData[m_size] = '\0';
}

void WritableString::AppendChar(char ch)
{
	if(!Reserve(m_size + 1))
	{
		ASSERT(false);
		return;
	}

	m_pData[m_size] = ch;
	m_size++;
	m_pData[m_size] = '\0';
}

WritableString& WritableString::operator +=(int32_t number)
{
	char str[10 + 2];
	const int len = snprintf(str, sizeof(str), "%d", number);
	if(len > 0)
	{
		Append(String(str, len));
	}
	return *this;
}

WritableString& WritableString::operator +=(uint32_t number)
{
	char str[10 + 2];
	const int len = snprintf(str, sizeof(str), "%u", number);
	if(len > 0)
	{
		Append(String(str, len));
	}
	return *this;
}

WritableString& WritableString::operator +=(int64_t number)
{
	char str[20 + 2];
	const int len = snprintf(str, sizeof(str), "%lld", number);
	if(len > 0)
	{
		Append(String(str, len));
	}
	return *this;
}

WritableString& WritableString::operator +=(uint64_t number)
{
	char str[20 + 2];
	const int len = snprintf(str, sizeof(str), "%llu", number);
	if(len > 0)
	{
		Append(String(str, len));
	}
	return *this;
}

WritableString& WritableString::operator +=(float number)
{
	char str[32];
	const int len = snprintf(str, sizeof(str), "%.3f", number);
	if(len > 0)
	{
		Append(String(str, len));
	}
	return *this;
}

WritableString& WritableString::operator =(const String& string)
{
	Set(string.Str(), string.Length());
	return *this;
}

WritableString& WritableString::operator =(const WritableString& string)
{
	Set(string.Str(), string.Length());
	return *this;
}

void WritableString::Set(const char* pStr, size_t numChars)
{
	ASSERT(pStr != nullptr);
	ASSERT(m_pData != pStr);	// It's not allowed to copy from ourself

	if(!Reserve(numChars))
	{
		ASSERT(false);
		return;
	}

	std::memcpy(m_pData, pStr, numChars);
	m_size = numChars;
	m_pData[m_size] = '\0';
}

void WritableString::Trim()
{
	auto IsWhiteSpace = [](char c)
	{
		return (c == ' ' || c == '\t' || c == '\n' || c == '\r');
	};

	// Remove from end
	while(m_size > 0 && IsWhiteSpace(m_pData[m_size - 1]))
	{
		m_size--;
	}
	m_pData[m_size] = '\0';

	// Remove from beginning
	char* pCh = m_pData;
	while(*pCh != '\0' && IsWhiteSpace(*pCh))
	{
		++pCh;
	}

	if(pCh != m_pData)
	{
		m_size -= pCh - m_pData;
		std::memmove(m_pData, pCh, m_size + 1);
	}
}

void WritableString::ToLower()
{
	char* pCh = m_pData;
	while(*pCh != '\0')
	{
		constexpr uint32_t rangeBegin = 'A';
		constexpr uint32_t rangeSize = 'Z' - 'A';
		if(static_cast<uint32_t>(*pCh) - rangeBegin <= rangeSize)
		{
			*pCh |= 0b00100000;	// Add 32 by setting the 6th bit
		}

		pCh++;
	}
}

void WritableString::ToUpper()
{
	char* pCh = m_pData;
	while(*pCh != '\0')
	{
		constexpr uint32_t rangeBegin = 'a';
		constexpr uint32_t rangeSize = 'z' - 'a';
		if(static_cast<uint32_t>(*pCh) - rangeBegin <= rangeSize)
		{
			*pCh &= 0b11011111;	// Subtract 32 by clearing the 6th bit
		}

		pCh++;
	}
}

char* WritableString::AsCharBufferWithSize(size_t newSize)
{
	if(Reserve(newSize))
	{
		m_size = newSize;
		m_pData[m_size] = '\0';
		return m_pData;
	}

	return nullptr;
}

void WritableString::Sprintf(const char* pFormatStr, ...)
{
	std::va_list args;
	va_start(args, pFormatStr);
	Sprintf_internal(0, pFormatStr, args);
	va_end(args);
}

void WritableString::SprintfAppend(const char* pFormatStr, ...)
{
	std::va_list args;
	va_start(args, pFormatStr);
	Sprintf_internal(m_size, pFormatStr, args);
	va_end(args);
}

void WritableString::Sprintf_internal(size_t pos, const char* pFormatStr, std::va_list args)
{
	ASSERT(pFormatStr != nullptr);

	std::va_list argsCopy;
	va_copy(argsCopy, args);
	const int requiredSize = std::vsnprintf(nullptr, 0, pFormatStr, argsCopy);
	va_end(argsCopy);

	if(requiredSize <= 0)
	{
		return;
	}

	const size_t newSize = pos + requiredSize;
	if(Reserve(newSize))
	{
		m_size = newSize;
		std::vsnprintf(m_pData + pos, requiredSize + 1, pFormatStr, args);
	}
}

void WritableString::Replace_Internal(size_t pos, size_t numChars, const String& replaceTo)
{
	std::memmove(m_pData + pos + replaceTo.Length(), m_pData + pos + numChars, m_size - pos - numChars + 1);
	m_size += replaceTo.Length() - numChars;

	std::memcpy(m_pData + pos, replaceTo.Str(), replaceTo.Length());
}
