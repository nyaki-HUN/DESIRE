#pragma once

#include "Engine/Core/String/String.h"

class WritableString : public String
{
public:
	// Erases the contents of the string, which becomes an empty string
	void Clear();

	// Insert characters
	void Insert(size_t pos, const char* pStr, size_t numChars);
	void Insert(size_t pos, const String& string)					{ Insert(pos, string.Str(), string.Length()); }

	template<size_t SIZE>
	void Insert(size_t pos, const char(&str)[SIZE])					{ Insert(pos, str, SIZE - 1); }

	// Remove characters
	void RemoveFrom(size_t pos, size_t numChars = SIZE_MAX);
	void RemoveFromEnd(size_t numChars);

	// Truncate the string at a given position and readjust the length
	void TruncateAt(size_t pos);

	// Replace characters in string
	void Replace(const String& search, const String& replaceTo);
	void ReplaceAll(const String& search, const String& replaceTo);
	void ReplaceAllChar(char search, char replaceTo);

	// Append characters
	void Append(const String& string);
	void AppendChar(char ch);
	WritableString& operator +=(const String& string)					{ Append(string); return *this; }

	// Append numbers
	WritableString& operator +=(int32_t number);
	WritableString& operator +=(uint32_t number);
	WritableString& operator +=(int64_t number);
	WritableString& operator +=(uint64_t number);
	WritableString& operator +=(float number);

	// Set a new value to the string, replacing its current contents
	WritableString& operator =(const String& string);
	WritableString& operator =(const WritableString& string);
	void Set(const char* pStr, size_t numChars);

	// Remove whitespaces from both the beginning and end of the string
	void Trim();

	// Convert the string to lowercase (in place)
	void ToLower();

	// Convert the string to uppercase (in place)
	void ToUpper();

	char* AsCharBufferWithSize(size_t newSize);

	// Write formatted data into the string from variable argument list
	void Sprintf(const char* pFormatStr, ...);
	void SprintfAppend(const char* pFormatStr, ...);

	// Requests the string capacity to be (at least) as big to hold numChars characters. Returns true on success
	virtual bool Reserve(size_t numChars) = 0;

protected:
	void Sprintf_internal(size_t pos, const char* pFormatStr, std::va_list args);
	void Replace_Internal(size_t pos, size_t numChars, const String& replaceTo);
};
