#pragma once

#include <stdint.h>

class String
{
public:
	static const size_t INVALID_POS = (size_t)-1;
	static const String emptyString;

	String();
	String(const String& string);
	String(String&& string);
	String(const char *str, size_t size);

	template<size_t SIZE>
	String(const char(&str)[SIZE])
	{
		InitWithData(str, SIZE - 1);
	}

	~String();

	void ToLower();
	void ToUpper();

	// Remove whitespaces from both the beginning and end of the string
	void Trim();

	// Insert characters
	void Insert(size_t startIndex, const char *str, size_t numChars);

	inline void Insert(size_t startIndex, const String& string)
	{
		Insert(startIndex, string.c_str(), string.size);
	}

	template<size_t SIZE>
	void Insert(size_t startIndex, const char(&str)[SIZE])
	{
		Insert(startIndex, str, SIZE - 1);
	}

	// Remove characters
	void RemoveFrom(size_t startIndex, size_t numChars = SIZE_MAX);
	void RemoveFromEnd(size_t numChars);

	// Find character(s) in string
	size_t Find(const String& search, size_t startIndex = 0) const;
	size_t Find(const char *search, size_t startIndex = 0) const;
	size_t Find(char search, size_t startIndex = 0) const;
	size_t FindLast(const String& search) const;
	size_t FindLast(const char *search) const;
	size_t FindLast(char search) const;

	// Replace characters in string
	void Replace(const String& search, const String& replaceTo);
	void Replace(const char *search, const char *replaceTo);
	void ReplaceAll(const String& search, const String& replaceTo);
	void ReplaceAll(const char *search, const char *replaceTo);
	void ReplaceAllChar(char search, char replaceTo);

	// Generate substring
	String SubString(size_t startIndex, size_t numChars = SIZE_MAX) const;

	// Returns the data (C string equivalent)
	const char* c_str() const;

	// Returns the number of characters
	size_t Length() const;

	// Returns the number of UTF8 characters
	size_t LengthUTF8() const;

	// Returns whether the string is empty
	inline bool IsEmpty() const
	{
		return (Length() == 0);
	}

	// Convert string to numeric value
	int IntValue() const;
	float FloatValue() const;

	String& operator =(const String& string);
	String& operator =(String&& string);

	// Append characters
	void Append(const char *str, size_t numChars);

	inline String operator +(const String& string) const
	{
		String newString(*this);
		newString.Append(string.data, string.size);
		return newString;
	}

	template<size_t SIZE>
	inline String operator +(const char(&str)[SIZE]) const
	{
		String newString(*this);
		newString.Append(str, SIZE - 1);
		return newString;
	}

	inline String& operator +=(const String& string)
	{
		Append(string.data, string.size);
		return *this;
	}

	template<size_t SIZE>
	inline String& operator +=(const char(&str)[SIZE])
	{
		Append(str, SIZE - 1);
		return *this;
	}

	// Append numbers
	String& operator +=(int32_t number);
	String& operator +=(uint32_t number);
	String& operator +=(int64_t number);
	String& operator +=(uint64_t number);
	String& operator +=(float number);

	// Compare strings
	int Compare(const String& string) const;
	int Compare(const char *str) const;
	int CompareIgnoreCase(const String& string) const;
	int CompareIgnoreCase(const char *str) const;

	inline bool operator <(const String& string) const
	{
		return Compare(string) < 0;
	}

	inline bool Equals(const String& string) const
	{
		return (Compare(string.data) == 0);
	}
	inline bool Equals(const char *str) const
	{
		return (Compare(str) == 0);
	}

	inline bool EqualsIgnoreCase(const String& string) const
	{
		return (CompareIgnoreCase(string.data) == 0);
	}
	inline bool EqualsIgnoreCase(const char *str) const
	{
		return (CompareIgnoreCase(str) == 0);
	}

	// Check if the string begins with the given prefix
	bool StartsWith(const char *prefix, size_t numChars) const;

	inline bool StartsWith(const String& prefix) const
	{
		return StartsWith(prefix.c_str(), prefix.size);
	}

	template<size_t SIZE>
	bool StartsWith(const char(&prefix)[SIZE]) const
	{
		return StartsWith(prefix, SIZE - 1);
	}

	// Check if the string ends with the given suffix
	bool EndsWith(const char *suffix, size_t numChars) const;
	bool EndsWith(char suffix) const;

	inline bool EndsWith(const String& prefix) const
	{
		return EndsWith(prefix.c_str(), prefix.size);
	}

	template<size_t SIZE>
	bool EndsWith(const char(&prefix)[SIZE]) const
	{
		return EndsWith(prefix, SIZE - 1);
	}

	// Create string with format
	static String CreateFormattedString(const char *format, ...);
	static String CreateFromInt(int num);

private:
	void Replace_Internal(const char *search, size_t searchLen, const char *replaceTo, size_t replaceToLen, bool all);
	void EnsureSize(size_t size, bool keepOld);
	void InitWithData(const char *data, size_t size);

	static const size_t kStackSize = 16;

	char *data = staticContent;
	size_t size = 0;
	char staticContent[kStackSize] = {};
};
