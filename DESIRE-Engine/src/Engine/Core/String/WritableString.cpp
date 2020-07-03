#include "Engine/stdafx.h"
#include "Engine/Core/String/WritableString.h"

void WritableString::Clear()
{
	size = 0;
	data[size] = '\0';
}

void WritableString::Insert(size_t startIndex, const char* pStr, size_t numChars)
{
	ASSERT(pStr != nullptr);

	if(startIndex > size)
	{
		return;
	}

	if(!Reserve(size + numChars))
	{
		ASSERT(false);
		return;
	}

	memmove(data + startIndex + numChars, data + startIndex, size - startIndex + 1);
	memcpy(data + startIndex, pStr, numChars);
	size += numChars;
}

void WritableString::RemoveFrom(size_t startIndex, size_t numChars)
{
	if(startIndex >= size)
	{
		return;
	}

	numChars = std::min(numChars, size - startIndex);

	memmove(data + startIndex, data + startIndex + numChars, size - startIndex - numChars + 1);
	size -= numChars;
}

void WritableString::RemoveFromEnd(size_t numChars)
{
	size -= (numChars <= size) ? numChars : size;
	data[size] = '\0';
}

void WritableString::TruncateAt(size_t startIndex)
{
	if(startIndex >= size)
	{
		return;
	}

	data[startIndex] = '\0';
	size = startIndex;
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
		data[foundPos] = replaceTo;
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

	memcpy(data + size, string.Str(), numChars);
	size += numChars;
	data[size] = '\0';
}

void WritableString::AppendChar(char ch)
{
	if(!Reserve(size + 1))
	{
		ASSERT(false);
		return;
	}

	data[size] = ch;
	size++;
	data[size] = '\0';
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
	ASSERT(data != pStr);	// It's not allowed to copy from ourself

	if(!Reserve(numChars))
	{
		ASSERT(false);
		return;
	}

	memcpy(data, pStr, numChars);
	size = numChars;
	data[size] = '\0';
}

void WritableString::Trim()
{
	auto IsWhiteSpace = [](char c)
	{
		return (c == ' ' || c == '\t' || c == '\n' || c == '\r');
	};

	// Remove from end
	while(size > 0 && IsWhiteSpace(data[size - 1]))
	{
		size--;
	}
	data[size] = '\0';

	// Remove from beginning
	char* ch = data;
	while(*ch != '\0' && IsWhiteSpace(*ch))
	{
		++ch;
	}

	if(ch != data)
	{
		size -= ch - data;
		memmove(data, ch, size + 1);
	}
}

void WritableString::ToLower()
{
	char* ch = data;
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
	char* ch = data;
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
		data[size] = '\0';
		return data;
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
		vsnprintf(data + pos, requiredSize + 1, pFormatStr, args);
	}
}

void WritableString::Replace_Internal(size_t pos, size_t numChars, const String& replaceTo)
{
	memmove(data + pos + replaceTo.Length(), data + pos + numChars, size - pos - numChars + 1);
	size += replaceTo.Length() - numChars;

	memcpy(data + pos, replaceTo.Str(), replaceTo.Length());
}
