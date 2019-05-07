#pragma once

#include "Engine/Core/String/String.h"

class WritableString : public String
{
public:
	WritableString();

	void ToLower();
	void ToUpper();

	// Remove whitespaces from both the beginning and end of the string
	void Trim();

	// Assign a new value to the string, replacing its current contents
	void Assign(const char *str, size_t numChars);

	// Erases the contents of the string, which becomes an empty string
	void Clear();

	// Insert characters
	void Insert(size_t startIndex, const char *str, size_t numChars);
	void Insert(size_t startIndex, const String& string)				{ Insert(startIndex, string.Str(), string.Length()); }

	template<size_t SIZE>
	void Insert(size_t startIndex, const char(&str)[SIZE])				{ Insert(startIndex, str, SIZE - 1); }

	// Remove characters
	void RemoveFrom(size_t startIndex, size_t numChars = SIZE_MAX);
	void RemoveFromEnd(size_t numChars);

	// Replace characters in string
	void Replace(const String& search, const String& replaceTo);
	void ReplaceAll(const String& search, const String& replaceTo);
	void ReplaceAllChar(char search, char replaceTo);

	// Append characters
	void Append(const char *str, size_t numChars);

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

protected:
	virtual bool Reserve(size_t newSize) = 0;

	void Replace_Internal(size_t pos, size_t numChars, const String& replaceTo);
};
