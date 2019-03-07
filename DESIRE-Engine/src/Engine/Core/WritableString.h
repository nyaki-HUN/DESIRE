#pragma once

#include "Engine/Core/String.h"

#include <cstdarg>		// For std::va_list

class WritableString : public String
{
public:
	WritableString();
	WritableString(const WritableString& string);

	void ToLower();
	void ToUpper();

	// Remove whitespaces from both the beginning and end of the string
	void Trim();

	// Insert characters
	void Insert(size_t startIndex, const char *str, size_t numChars);

	void Insert(size_t startIndex, const String& string)
	{
		Insert(startIndex, string.Str(), string.Length());
	}

	template<size_t SIZE>
	void Insert(size_t startIndex, const char(&str)[SIZE])
	{
		Insert(startIndex, str, SIZE - 1);
	}

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

	// Set formatted string from variable argument list
	bool Format(const char *format, ...);

protected:
	virtual bool Reserve(size_t newSize) = 0;

	bool Format_Internal(const char *format, std::va_list argList);
	void Replace_Internal(const char *search, size_t searchLen, const char *replaceTo, size_t replaceToLen, bool all);
	void InitWithData(const char *data, size_t size);
};
