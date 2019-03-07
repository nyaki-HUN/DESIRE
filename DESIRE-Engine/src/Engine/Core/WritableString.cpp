#include "Engine/stdafx.h"
#include "Engine/Core/WritableString.h"
#include "Engine/Core/StrUtils.h"

WritableString::WritableString()
{

}

WritableString::WritableString(const WritableString& string)
{
	InitWithData(string.Str(), string.Length());
}

void WritableString::ToLower()
{
	StrUtils::ToLower(data);
}

void WritableString::ToUpper()
{
	StrUtils::ToUpper(data);
}

void WritableString::Trim()
{
	// Remove from end
	while(size > 0 && StrUtils::IsSpace(data[size - 1]))
	{
		size--;
	}
	data[size] = '\0';

	// Remove from beginning
	const char *newData = StrUtils::SkipSpaces(data);
	if(newData != data)
	{
		size -= newData - data;
		memmove(data, newData, size + 1);
	}
}

void WritableString::Insert(size_t startIndex, const char *str, size_t numChars)
{
	ASSERT(str != nullptr);

	if(startIndex > size)
	{
		return;
	}

	const bool hasEnoughSize = Reserve(size + numChars);
	ASSERT(hasEnoughSize);
	if(hasEnoughSize)
	{
		memmove(data + startIndex + numChars, data + startIndex, size - startIndex + 1);
		memcpy(data + startIndex, str, numChars);
		size += numChars;
	}
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
	Replace_Internal(search, replaceTo, false);
}

void WritableString::ReplaceAll(const String& search, const String& replaceTo)
{
	Replace_Internal(search, replaceTo, true);
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

	const bool hasEnoughSize = Reserve(size + numChars);
	ASSERT(hasEnoughSize);
	if(hasEnoughSize)
	{
		memcpy(data + size, str, numChars);
		size += numChars;
		data[size] = '\0';
	}
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

bool WritableString::Format(const char *format, ...)
{
	std::va_list args;
	va_start(args, format);
	const bool result = Format_Internal(format, args);
	va_end(args);

	return result;
}

bool WritableString::Format_Internal(const char *format, std::va_list args)
{
	ASSERT(format != nullptr);

	const int formattedSize = vsnprintf(data, size, format, args);
	return (formattedSize >= 0 && (size_t)formattedSize < size);
}

void WritableString::Replace_Internal(const String& search, const String& replaceTo, bool all)
{
	size_t pos = 0;
	for(;;)
	{
		pos = Find(search, pos);
		if(pos == kInvalidPos)
		{
			break;
		}

		RemoveFrom(pos, search.Length());
		Insert(pos, replaceTo);
		pos += replaceTo.Length();

		if(!all)
		{
			break;
		}
	}
}

void WritableString::InitWithData(const char *newData, size_t newSize)
{
	ASSERT(newData != nullptr);

	const bool hasEnoughSize = Reserve(newSize);
	ASSERT(hasEnoughSize);
	if(hasEnoughSize)
	{
		memcpy(data, newData, newSize);
		size = newSize;
		data[size] = '\0';
	}
}
