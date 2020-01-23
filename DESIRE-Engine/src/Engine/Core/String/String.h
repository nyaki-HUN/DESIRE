#pragma once

#include <stdint.h>

class String
{
public:
	static constexpr size_t kInvalidPos = (size_t)-1;
	static const String kEmptyString;

	String(const char* str, size_t size);

	template<size_t SIZE>
	String(const char(&str)[SIZE])
		: String(str, SIZE - 1)
	{
	}

	virtual ~String() {}

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

	// Convert string to number
	int32_t AsInt32() const;
	int64_t AsInt64() const;
	uint32_t AsUint32() const;
	uint64_t AsUint64() const;
	float AsFloat() const;
	double AsDouble() const;

	// Compare strings
	int Compare(const String& string) const;
	int CompareIgnoreCase(const String& string) const;

	bool Equals(const String& string) const;
	bool EqualsIgnoreCase(const String& string) const;

	bool operator ==(const String& string) const		{ return Equals(string); }
	bool operator !=(const String& string) const		{ return !Equals(string); }
	bool operator <(const String& string) const			{ return (Compare(string) < 0); }

	// Check if the string begins with the given prefix
	bool StartsWith(const String& prefix) const;
	bool StartsWith(char prefix) const;

	// Check if the string ends with the given suffix
	bool EndsWith(const String& suffix) const;
	bool EndsWith(char suffix) const;

protected:
	String() {}

	char* data = nullptr;
	size_t size = 0;

private:
	// Prevent copy and move
	String(const String&) = delete;
	String(String&&) = delete;
	String& operator =(const String&) = delete;
	String& operator =(String&&) = delete;
};
