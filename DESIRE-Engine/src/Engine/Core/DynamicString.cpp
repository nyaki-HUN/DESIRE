#include "Engine/stdafx.h"
#include "Engine/Core/DynamicString.h"

#include <stdarg.h>

DynamicString::DynamicString(size_t numReservedChars)
{
	Reserve(numReservedChars);
}

DynamicString::DynamicString(const char *str, size_t size)
{
	InitWithData(str, size);
}

DynamicString::DynamicString(const String& string)
{
	InitWithData(string.Str(), string.Length());
}

DynamicString::DynamicString(const DynamicString& string)
{
	InitWithData(string.Str(), string.Length());
}

DynamicString::DynamicString(DynamicString&& string)
{
	data = string.data;
	size = string.size;
	preallocatedSize = string.preallocatedSize;

	string.data = "";
	string.size = 0;
	string.preallocatedSize = 0;
}

DynamicString::~DynamicString()
{
	if(preallocatedSize != 0)
	{
		free(data);
	}
}

DynamicString& DynamicString::operator =(const String& string)
{
	ASSERT(this != &string);	// It's not allowed to copy from ourself
	InitWithData(string.Str(), string.Length());
	return *this;
}

DynamicString& DynamicString::operator =(const DynamicString& string)
{
	ASSERT(this != &string);	// It's not allowed to copy from ourself
	InitWithData(string.Str(), string.Length());
	return *this;
}

DynamicString& DynamicString::operator =(DynamicString&& string)
{
	ASSERT(this != &string);	// It's not allowed to copy from ourself

	if(preallocatedSize != 0)
	{
		free(data);
	}

	data = string.data;
	size = string.size;
	preallocatedSize = string.preallocatedSize;

	string.data = nullptr;
	string.size = 0;
	string.preallocatedSize = 0;

	return *this;
}

DynamicString DynamicString::SubString(size_t startIndex, size_t numChars) const
{
	if(startIndex >= size)
	{
		return DynamicString();
	}

	return DynamicString(&data[startIndex], std::min(numChars, size - startIndex));
}

DynamicString DynamicString::CreateFormattedString(const char *format, ...)
{
	char str[1024];

	va_list args;
	va_start(args, format);
	const int size = vsnprintf(str, sizeof(str), format, args);
	ASSERT(size < sizeof(str));
	va_end(args);

	return DynamicString(str, (size_t)size);
}

bool DynamicString::Reserve(size_t numChars)
{
	if(preallocatedSize < numChars)
	{
		if(preallocatedSize == 0)
		{
			data = (char*)malloc(numChars + 1);
		}
		else
		{
			data = (char*)realloc(data, numChars + 1);
		}

		preallocatedSize = numChars;
	}

	return true;
}
