#include "Engine/stdafx.h"
#include "Engine/Core/String/WritableString.h"

#include <cctype>	// for std::tolower() and std::toupper()

void WritableString::Assign(const char *str, size_t numChars)
{
	ASSERT(str != nullptr);
	ASSERT(data != str);	// It's not allowed to copy from ourself

	if(!Reserve(numChars))
	{
		ASSERT(false);
		return;
	}

	memcpy(data, str, numChars);
	size = numChars;
	data[size] = '\0';
}

void WritableString::Clear()
{
	size = 0;
	data[size] = '\0';
}

void WritableString::Insert(size_t startIndex, const char *str, size_t numChars)
{
	ASSERT(str != nullptr);

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
	memcpy(data + startIndex, str, numChars);
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

void WritableString::Replace(const String& search, const String& replaceTo)
{
	const size_t firstPos = Find(search);
	if(firstPos == kInvalidPos)
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

	Replace_Internal(firstPos, search.Length(), replaceTo);
}

void WritableString::ReplaceAll(const String& search, const String& replaceTo)
{
	const size_t firstPos = Find(search);
	if(firstPos == kInvalidPos)
	{
		return;
	}

	const int64_t numCharsToMove = replaceTo.Length() - search.Length();
	if(numCharsToMove > 0)
	{
		size_t count = 0;
		size_t pos = firstPos;
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

	size_t pos = firstPos;
	do
	{
		Replace_Internal(pos, search.Length(), replaceTo);
		pos = Find(search, pos + replaceTo.Length());
	} while(pos != kInvalidPos);
}

void WritableString::ReplaceAllChar(char search, char replaceTo)
{
	ASSERT(search != '\0');

	char *dataTmp = data;
	for(;;)
	{
		char *foundPtr = strchr(dataTmp, search);
		if(foundPtr == nullptr)
		{
			break;
		}

		*foundPtr = replaceTo;
		dataTmp = foundPtr + 1;
	}
}

void WritableString::Append(const char *str, size_t numChars)
{
	ASSERT(str != nullptr);

	if(!Reserve(size + numChars))
	{
		ASSERT(false);
		return;
	}

	memcpy(data + size, str, numChars);
	size += numChars;
	data[size] = '\0';
}

WritableString& WritableString::operator +=(int32_t number)
{
	char str[10 + 2];
	const int len = snprintf(str, sizeof(str), "%d", number);
	if(len > 0)
	{
		Append(str, len);
	}
	return *this;
}

WritableString& WritableString::operator +=(uint32_t number)
{
	char str[10 + 2];
	const int len = snprintf(str, sizeof(str), "%u", number);
	if(len > 0)
	{
		Append(str, len);
	}
	return *this;
}

WritableString& WritableString::operator +=(int64_t number)
{
	char str[20 + 2];
	const int len = snprintf(str, sizeof(str), "%lld", number);
	if(len > 0)
	{
		Append(str, len);
	}
	return *this;
}

WritableString& WritableString::operator +=(uint64_t number)
{
	char str[20 + 2];
	const int len = snprintf(str, sizeof(str), "%llu", number);
	if(len > 0)
	{
		Append(str, len);
	}
	return *this;
}

WritableString& WritableString::operator +=(float number)
{
	char str[32];
	const int len = snprintf(str, sizeof(str), "%.3f", number);
	if(len > 0)
	{
		Append(str, len);
	}
	return *this;
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
	char *ch = data;
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
	char *ch = data;
	while(*ch != '\0')
	{
		*ch = (char)std::tolower(*ch);
		ch++;
	}
}

void WritableString::ToUpper()
{
	char *ch = data;
	while(*ch != '\0')
	{
		*ch = (char)std::toupper(*ch);
		ch++;
	}
}

void WritableString::Sprintf(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	Sprintf_internal(format, args);
	va_end(args);
}

void WritableString::Sprintf_internal(const char *format, va_list args)
{
	ASSERT(format != nullptr);

	va_list argsCopy;
	va_copy(argsCopy, args);
	const int requiredSize = vsnprintf(nullptr, 0, format, argsCopy);
	va_end(argsCopy);

	if(requiredSize > 0 && Reserve(requiredSize))
	{
		size = requiredSize;
		vsnprintf(data, requiredSize + 1, format, args);
	}
}

void WritableString::Replace_Internal(size_t pos, size_t numChars, const String& replaceTo)
{
	memmove(data + pos + replaceTo.Length(), data + pos + numChars, size - pos - numChars + 1);
	size += replaceTo.Length() - numChars;

	memcpy(data + pos, replaceTo.Str(), replaceTo.Length());
}
