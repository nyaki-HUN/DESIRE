#pragma once

#include "Engine/Core/String/WritableString.h"

template<size_t STACK_SIZE>
class StackString : public WritableString
{
public:
	StackString()
	{
		data = charBuffer;
	}

	StackString(const char* str, size_t size)
	{
		data = charBuffer;
		Set(str, size);
	}

	StackString(const String& string)					: StackString(string.Str(), string.Length()) {}
	StackString(const StackString<STACK_SIZE>& string)	: StackString(string.Str(), string.Length()) {}

	StackString<STACK_SIZE>& operator =(const String& string)	{ Set(string.Str(), string.Length()); return *this; }

	// Create formatted string from variable argument list
	static StackString<STACK_SIZE> Format(const char* format, ...)
	{
		StackString<STACK_SIZE> string;

		std::va_list args;
		va_start(args, format);
		string.Sprintf_internal(0, format, args);
		va_end(args);

		return string;
	}

private:
	bool Reserve(size_t numChars) override
	{
		return (numChars < STACK_SIZE);
	}

	char charBuffer[STACK_SIZE] = {};
};
