#include "Engine/stdafx.h"
#include "Engine/Core/String/DynamicString.h"

DynamicString::DynamicString(size_t numReservedChars)
{
	Reserve(numReservedChars);
	memset(data, 0, preallocatedSize * sizeof(char));
}

DynamicString::DynamicString(const char* str, size_t size)
{
	Assign(str, size);
}

DynamicString::DynamicString(DynamicString&& string)
{
	data = string.data;
	size = string.size;
	preallocatedSize = string.preallocatedSize;

	string.data = nullptr;
	string.size = 0;
	string.preallocatedSize = 0;
}

DynamicString::~DynamicString()
{
	free(data);
}

DynamicString& DynamicString::operator =(DynamicString&& string)
{
	ASSERT(this != &string);	// It's not allowed to copy from ourself

	free(data);

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

DynamicString DynamicString::Format(const char* format, ...)
{
	DynamicString string;

	va_list args;
	va_start(args, format);
	string.Sprintf_internal(0, format, args);
	va_end(args);

	return string;
}

bool DynamicString::Reserve(size_t numChars)
{
	if(preallocatedSize < numChars + 1)
	{
		preallocatedSize = numChars + 1;
		data = static_cast<char*>(realloc(data, preallocatedSize * sizeof(char)));
	}

	return true;
}
