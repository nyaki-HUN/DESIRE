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
	String(const char *s);
	String(const char *data, size_t size);
	~String();

	void ToLower();
	void ToUpper();

	// Remove whitespaces from both the beginning and end of the string
	void Trim();

	// Append characters
	void Append(const String& string);
	void Append(const char *s);
	void Append(const char *s, size_t numChars);

	// Insert characters
	void Insert(size_t startIndex, const String& string);
	void Insert(size_t startIndex, const char *s);
	void Insert(size_t startIndex, const char *s, size_t numChars);

	// Remove characters
	void Remove(size_t startIndex, size_t numChars);
	void RemoveFromEnd(size_t numChars);

	// Find character(s) in string
	size_t Find(const String& search, size_t startIndex = 0) const;
	size_t Find(const char *search, size_t startIndex = 0) const;
	size_t Find(char search, size_t startIndex = 0) const;
	size_t FindLast(const String& search) const;
	size_t FindLast(const char *search) const;
	size_t FindLast(char search) const;

	// Replace characters in string
	inline void Replace(const String& search, const String& replaceTo)
	{
		Replace_Internal(search.c_str(), replaceTo.c_str(), false);
	}
	inline void Replace(const char *search, const char *replaceTo)
	{
		Replace_Internal(search, replaceTo, false);
	}

	inline void ReplaceAll(const String& search, const String& replaceTo)
	{
		Replace_Internal(search.c_str(), replaceTo.c_str(), true);
	}
	inline void ReplaceAll(const char *search, const char *replaceTo)
	{
		Replace_Internal(search, replaceTo, true);
	}

	// Generate substring
	String SubString(size_t startIndex, size_t numChars = INVALID_POS) const;

	// Returns the number of characters
	size_t Length() const;

	// Returns the number of UTF8 characters
	size_t LengthUTF8() const;

	// Returns the data (C string equivalent)
	const char* c_str() const;

	// Convert string to numeric value
	int IntValue() const;
	float FloatValue() const;

	String& operator =(const String& string);
	String& operator =(String&& string);
	String& operator =(const char *s);

	inline String operator +(const String& string) const
	{
		return (String(*this) += string);
	}

	inline String operator +(const char *s) const
	{
		return (String(*this) += s);
	}

	// Append characters
	inline String& operator +=(const String& string)
	{
		Append(string);
		return *this;
	}
	inline String& operator +=(const char *s)
	{
		Append(s);
		return *this;
	}

	// Append numbers
	String& operator +=(int32_t number);
	String& operator +=(uint32_t number);
	String& operator +=(int64_t number);
	String& operator +=(uint64_t number);
	String& operator +=(float number);

	// Append bool value
	String& operator +=(bool b);

	// Compare strings
	int Compare(const String& string) const;
	int Compare(const char *s) const;
	int CompareIgnoreCase(const String& string) const;
	int CompareIgnoreCase(const char *s) const;

	inline bool operator <(const String& string) const
	{
		return Compare(string) < 0;
	}

	inline bool Equals(const String& string) const
	{
		return (Compare(string.data) == 0);
	}
	inline bool Equals(const char *s) const
	{
		return (Compare(s) == 0);
	}

	inline bool EqualsIgnoreCase(const String& string) const
	{
		return (CompareIgnoreCase(string.data) == 0);
	}
	inline bool EqualsIgnoreCase(const char *s) const
	{
		return (CompareIgnoreCase(s) == 0);
	}

	bool StartsWith(const String& prefix) const;
	bool StartsWith(const char *prefix) const;
	bool EndsWith(const String& suffix) const;
	bool EndsWith(const char *suffix) const;
	bool EndsWith(char suffix) const;

	static String CreateFormattedString(const char *format, ...);
	static String CreateFromInt(int num);

private:
	void Replace_Internal(const char *search, const char *replaceTo, bool all);
	void EnsureSize(size_t size, bool keepOld);
	void InitWithData(const char *data, size_t size);

	static const size_t kStackSize = 16;

	char *data;
	size_t size;
	char staticContent[kStackSize];
};
