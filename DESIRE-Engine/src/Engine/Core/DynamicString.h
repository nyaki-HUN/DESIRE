#pragma once

#include "Engine/Core/WritableString.h"

class DynamicString : public WritableString
{
public:
	DynamicString(size_t numReservedChars = 0);
	DynamicString(const char *str, size_t size);
	DynamicString(const String& string);
	DynamicString(const DynamicString& string);
	DynamicString(DynamicString&& string);
	~DynamicString() override;

	template<size_t SIZE>
	DynamicString(const char(&str)[SIZE])
	{
		InitWithData(str, SIZE - 1);
	}

	DynamicString& operator =(const String& string);
	DynamicString& operator =(const DynamicString& string);
	DynamicString& operator =(DynamicString&& string);

	// Generate substring
	DynamicString SubString(size_t startIndex, size_t numChars = SIZE_MAX) const;

	// Create string with format
	static DynamicString CreateFormattedString(const char *format, ...);

private:
	bool Reserve(size_t numChars) override;

	size_t preallocatedSize = 0;
};

DynamicString operator +(const String& a, const String& b)
{
	DynamicString newString(a.Length() + b.Length());
	newString += a;
	newString += b;
	return newString;
}

template<size_t SIZE>
DynamicString operator +(const String& string, const char(&str)[SIZE])
{
	DynamicString newString(string.Length() + SIZE - 1);
	newString += string;
	newString.Append(str, SIZE - 1);
	return newString;
}
