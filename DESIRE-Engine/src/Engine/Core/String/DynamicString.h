#pragma once

#include "Engine/Core/String/WritableString.h"

class DynamicString : public WritableString
{
public:
	DynamicString();
	DynamicString(const char* pStr, size_t size);
	DynamicString(const String& string)				: DynamicString(string.Str(), string.Length()) {}
	DynamicString(const DynamicString& string)		: DynamicString(string.Str(), string.Length()) {}
	DynamicString(DynamicString&& string);

	template<size_t SIZE>
	DynamicString(const char(&str)[SIZE])			: DynamicString(str, SIZE - 1) {}

	~DynamicString() override;

	DynamicString& operator =(const String& string)				{ Set(string.Str(), string.Length()); return *this; }
	DynamicString& operator =(const DynamicString& string)		{ Set(string.Str(), string.Length()); return *this; }
	DynamicString& operator =(DynamicString&& string);

	// Returns a newly constructed string which is initialized to a copy a substring of this string
	DynamicString SubString(size_t pos, size_t numChars = SIZE_MAX) const;

	// Resets the string to an empty string and deallocates its memory
	void Reset();

	// Requests the string capacity to be (at least) as big to hold numChars characters. Returns true on success
	bool Reserve(size_t numChars) override;

private:
	size_t m_preallocatedSize = 0;
};
