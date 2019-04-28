#pragma once

#include <stdint.h>

class String
{
public:
	static constexpr size_t kInvalidPos = (size_t)-1;
	static const String kEmptyString;

	template<size_t SIZE>
	String(const char(&str)[SIZE])
	{
		data = const_cast<char*>(str);
		size = SIZE - 1;
	}

	virtual ~String() = default;

	// Find character(s) in string
	size_t Find(const String& search, size_t startIndex = 0) const;
	size_t Find(char search, size_t startIndex = 0) const;
	size_t FindLast(const String& search) const;
	size_t FindLast(char search) const;

	// Returns the data (C string equivalent)
	const char* Str() const;

	// Returns the number of characters
	size_t Length() const;

	// Returns the number of UTF8 characters
	size_t LengthUTF8() const;

	// Returns whether the string is empty
	bool IsEmpty() const;

	// Convert string to numeric value
	int IntValue() const;
	float FloatValue() const;

	// Compare strings
	int Compare(const String& string) const;
	int CompareIgnoreCase(const String& string) const;

	bool Equals(const String& string) const				{ return (Compare(string) == 0); }
	bool EqualsIgnoreCase(const String& string) const	{ return (CompareIgnoreCase(string) == 0); }

	bool operator ==(const String& string) const		{ return (Compare(string) == 0); }
	bool operator !=(const String& string) const		{ return (Compare(string) != 0); }
	bool operator <(const String& string) const			{ return (Compare(string) < 0); }

	// Check if the string begins with the given prefix
	bool StartsWith(const String& prefix) const;
	bool StartsWith(const char *prefix, size_t numChars) const;

	// Check if the string ends with the given suffix
	bool EndsWith(const String& prefix) const;
	bool EndsWith(const char *suffix, size_t numChars) const;
	bool EndsWith(char suffix) const;

protected:
	String() = default;

	char *data = nullptr;
	size_t size = 0;

private:
	String(const String& string) = delete;
	String(String&& string) = delete;
	String& operator =(const String& string) = delete;
	String& operator =(String&& string) = delete;
};
