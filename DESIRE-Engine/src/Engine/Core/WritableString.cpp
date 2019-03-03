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

	const size_t oldSize = size;
	const bool hasEnoughSize = Reserve(size + numChars);
	ASSERT(hasEnoughSize);
	if(hasEnoughSize)
	{
		memmove(data + startIndex + numChars, data + startIndex, oldSize - startIndex + 1);
		memcpy(data + startIndex, str, numChars);
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
	Replace_Internal(search.Str(), search.Length(), replaceTo.Str(), replaceTo.Length(), false);
}

void WritableString::ReplaceAll(const String& search, const String& replaceTo)
{
	Replace_Internal(search.Str(), search.Length(), replaceTo.Str(), replaceTo.Length(), true);
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

	const size_t oldSize = size;
	const bool hasEnoughSize = Reserve(size + numChars);
	ASSERT(hasEnoughSize);
	if(hasEnoughSize)
	{
		memcpy(data + oldSize, str, numChars);
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

void WritableString::Replace_Internal(const char *search, size_t searchLen, const char *replaceTo, size_t replaceToLen, bool all)
{
	String newString;
	char *dataTmp = data;
	for(;;)
	{
		char *foundPtr = strstr(dataTmp, search);
		if(foundPtr == nullptr)
		{
			break;
		}

		*foundPtr = '\0';
		newString.Append(dataTmp, static_cast<size_t>(foundPtr - dataTmp));
		newString.Append(replaceTo, replaceToLen);
		dataTmp = foundPtr + searchLen;
		if(!all)
		{
			break;
		}
	}
	// Add remaining part
	newString.Append(dataTmp, strlen(dataTmp));

	// Move back the content from newString
	*this = std::move(newString);
}

void WritableString::InitWithData(const char *newData, size_t newSize)
{
	ASSERT(newData != nullptr);

	const bool hasEnoughSize = Reserve(newSize);
	ASSERT(hasEnoughSize);
	if(hasEnoughSize)
	{
		size = newSize;
		memcpy(data, newData, size);
		data[size] = '\0';
	}
}
