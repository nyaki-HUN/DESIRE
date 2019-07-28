#pragma once

#include "Engine/Core/String/WritableString.h"

class DynamicString : public WritableString
{
public:
	DynamicString(size_t numReservedChars = 0);
	DynamicString(const char* str, size_t size);
	DynamicString(const String& string)				: DynamicString(string.Str(), string.Length()) {}
	DynamicString(const DynamicString& string)		: DynamicString(string.Str(), string.Length()) {}
	DynamicString(DynamicString&& string);

	template<size_t SIZE>
	DynamicString(const char(&str)[SIZE])			: DynamicString(str, SIZE - 1) {}

	~DynamicString() override;

	DynamicString& operator =(const String& string)				{ Assign(string.Str(), string.Length()); return *this; }
	DynamicString& operator =(const DynamicString& string)		{ Assign(string.Str(), string.Length()); return *this; }
	DynamicString& operator =(DynamicString&& string);

	// Create a new string which is initialized to a copy a substring of this string
	DynamicString SubString(size_t startIndex, size_t numChars = SIZE_MAX) const;

	// Create formatted string from variable argument list
	static DynamicString Format(const char* format, ...);

private:
	bool Reserve(size_t numChars) override;

	size_t preallocatedSize = 0;
};

inline DynamicString operator +(const String& a, const String& b)
{
	DynamicString newString(a.Length() + b.Length());
	newString += a;
	newString += b;
	return newString;
}
