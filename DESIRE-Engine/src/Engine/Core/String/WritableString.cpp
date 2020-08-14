#include "Engine/stdafx.h"
#include "Engine/Core/String/WritableString.h"

void WritableString::Clear()
{
	size = 0;
	pData[size] = '\0';
}

void WritableString::Insert(size_t pos, const char* pStr, size_t numChars)
{
	ASSERT(pStr != nullptr);

	if(pos > size)
	{
		return;
	}

	if(!Reserve(size + numChars))
	{
		ASSERT(false);
		return;
	}

	memmove(pData + pos + numChars, pData + pos, size - pos + 1);
	memcpy(pData + pos, pStr, numChars);
	size += numChars;
}

void WritableString::RemoveFrom(size_t pos, size_t numChars)
{
	if(pos >= size)
	{
		return;
	}

	numChars = std::min(numChars, size - pos);

	memmove(pData + pos, pData + pos + numChars, size - pos - numChars + 1);
	size -= numChars;
}

void WritableString::RemoveFromEnd(size_t numChars)
{
	size -= (numChars <= size) ? numChars : size;
	pData[size] = '\0';
}

void WritableString::TruncateAt(size_t pos)
{
	if(pos >= size)
	{
		return;
	}

	pData[pos] = '\0';
	size = pos;
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
		if(!Reserve(size + numCharsToMove))
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

		if(!Reserve(size + count * numCharsToMove))
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
		pData[foundPos] = replaceTo;
		foundPos = Find(search, foundPos + 1);
	} while(foundPos != kInvalidPos);
}

void WritableString::Append(const String& string)
{
	const size_t numChars = string.Length();
	if(!Reserve(size + numChars))
	{
		ASSERT(false);
		return;
	}

	memcpy(pData + size, string.Str(), numChars);
	size += numChars;
	pData[size] = '\0';
}

void WritableString::AppendChar(char ch)
{
	if(!Reserve(size + 1))
	{
		ASSERT(false);
		return;
	}

	pData[size] = ch;
	size++;
	pData[size] = '\0';
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
	ASSERT(pData != pStr);	// It's not allowed to copy from ourself

	if(!Reserve(numChars))
	{
		ASSERT(false);
		return;
	}

	memcpy(pData, pStr, numChars);
	size = numChars;
	pData[size] = '\0';
}

void WritableString::Trim()
{
	auto IsWhiteSpace = [](char c)
	{
		return (c == ' ' || c == '\t' || c == '\n' || c == '\r');
	};

	// Remove from end
	while(size > 0 && IsWhiteSpace(pData[size - 1]))
	{
		size--;
	}
	pData[size] = '\0';

	// Remove from beginning
	char* ch = pData;
	while(*ch != '\0' && IsWhiteSpace(*ch))
	{
		++ch;
	}

	if(ch != pData)
	{
		size -= ch - pData;
		memmove(pData, ch, size + 1);
	}
}

void WritableString::ToLower()
{
	char* ch = pData;
	while(*ch != '\0')
	{
		constexpr uint32_t rangeBegin = 'A';
		constexpr uint32_t rangeSize = 'Z' - 'A';
		if(static_cast<uint32_t>(*ch) - rangeBegin <= rangeSize)
		{
			*ch |= 0b00100000;	// Add 32 by setting the 6th bit
		}

		ch++;
	}
}

void WritableString::ToUpper()
{
	char* ch = pData;
	while(*ch != '\0')
	{
		constexpr uint32_t rangeBegin = 'a';
		constexpr uint32_t rangeSize = 'z' - 'a';
		if(static_cast<uint32_t>(*ch) - rangeBegin <= rangeSize)
		{
			*ch &= 0b11011111;	// Subtract 32 by clearing the 6th bit
		}

		ch++;
	}
}

char* WritableString::AsCharBufferWithSize(size_t newSize)
{
	if(Reserve(newSize))
	{
		size = newSize;
		pData[size] = '\0';
		return pData;
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
	Sprintf_internal(size, pFormatStr, args);
	va_end(args);
}

void WritableString::Sprintf_internal(size_t pos, const char* pFormatStr, std::va_list args)
{
	ASSERT(pFormatStr != nullptr);

	std::va_list argsCopy;
	va_copy(argsCopy, args);
	const int requiredSize = vsnprintf(nullptr, 0, pFormatStr, argsCopy);
	va_end(argsCopy);

	if(requiredSize <= 0)
	{
		return;
	}

	const size_t newSize = pos + requiredSize;
	if(Reserve(newSize))
	{
		size = newSize;
		vsnprintf(pData + pos, requiredSize + 1, pFormatStr, args);
	}
}

void WritableString::Replace_Internal(size_t pos, size_t numChars, const String& replaceTo)
{
	memmove(pData + pos + replaceTo.Length(), pData + pos + numChars, size - pos - numChars + 1);
	size += replaceTo.Length() - numChars;

	memcpy(pData + pos, replaceTo.Str(), replaceTo.Length());
}
