#pragma once

#include "Engine/Core/String/String.h"

#include <cstdarg>		// for va_list

class WritableString : public String
{
public:
	// Assign a new value to the string, replacing its current contents
	void Assign(const char* str, size_t numChars);

	// Erases the contents of the string, which becomes an empty string
	void Clear();

	// Insert characters
	void Insert(size_t startIndex, const char* str, size_t numChars);
	void Insert(size_t startIndex, const String& string)				{ Insert(startIndex, string.Str(), string.Length()); }

	template<size_t SIZE>
	void Insert(size_t startIndex, const char(&str)[SIZE])				{ Insert(startIndex, str, SIZE - 1); }

	// Remove characters
	void RemoveFrom(size_t startIndex, size_t numChars = SIZE_MAX);
	void RemoveFromEnd(size_t numChars);

	// Truncate the string at a given position and readjust the length
	void TruncateAt(size_t startIndex);

	// Replace characters in string
	void Replace(const String& search, const String& replaceTo);
	void ReplaceAll(const String& search, const String& replaceTo);
	void ReplaceAllChar(char search, char replaceTo);

	// Append characters
	void Append(const char* str, size_t numChars);

	WritableString& operator +=(const String& string)
	{
		Append(string.Str(), string.Length());
		return *this;
	}

	// Append numbers
	WritableString& operator +=(int32_t number);
	WritableString& operator +=(uint32_t number);
	WritableString& operator +=(int64_t number);
	WritableString& operator +=(uint64_t number);
	WritableString& operator +=(float number);

	// Remove whitespaces from both the beginning and end of the string
	void Trim();

	// Convert the string to lowercase (in place)
	void ToLower();

	// Convert the string to uppercase (in place)
	void ToUpper();

	// Write formatted data into the string from variable argument list
	void Sprintf(const char* format, ...);
	void SprintfAppend(const char* format, ...);

protected:
	virtual bool Reserve(size_t numChars) = 0;

	void Sprintf_internal(size_t pos, const char* format, va_list args);
	void Replace_Internal(size_t pos, size_t numChars, const String& replaceTo);
};
