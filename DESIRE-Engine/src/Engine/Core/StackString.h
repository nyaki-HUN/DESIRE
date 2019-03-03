#pragma once

#include "Engine/Core/WritableString.h"

template<size_t STACK_SIZE>
class StackString : public WritableString
{
public:
	StackString()
	{
		data = charBuffer;
	}

	StackString(const char *str, size_t size)
	{
		data = charBuffer;
		InitWithData(str, size);
	}

	StackString(const String& string)
	{
		data = charBuffer;
		InitWithData(string.Str(), string.Length());
	}

	StackString<STACK_SIZE>& operator =(const String& string)
	{
		InitWithData(string.Str(), string.Length());
	}

	static StackString<STACK_SIZE> CreateFormattedString(const char *format, ...)
	{
		StackString<STACK_SIZE> string;

		std::va_list args;
		va_start(args, format);
		const bool result = string.Format_Internal(format, args);
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
