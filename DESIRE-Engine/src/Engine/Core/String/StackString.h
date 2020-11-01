#pragma once

#include "Engine/Core/String/WritableString.h"

template<size_t STACK_SIZE>
class StackString : public WritableString
{
public:
	StackString()
	{
		m_pData = m_charBuffer;
	}

	StackString(const char* pStr, size_t size)
	{
		m_pData = m_charBuffer;
		Set(pStr, size);
	}

	StackString(const String& string)					: StackString(string.Str(), string.Length()) {}
	StackString(const StackString<STACK_SIZE>& string)	: StackString(string.Str(), string.Length()) {}

	StackString<STACK_SIZE>& operator =(const String& string)					{ Set(string.Str(), string.Length()); return *this; }
	StackString<STACK_SIZE>& operator =(const StackString<STACK_SIZE>& string)	{ Set(string.Str(), string.Length()); return *this; }

	// Create formatted string from variable argument list
	static StackString<STACK_SIZE> Format(const char* pFormat, ...)
	{
		StackString<STACK_SIZE> string;

		std::va_list args;
		va_start(args, pFormat);
		string.Sprintf_internal(0, pFormat, args);
		va_end(args);

		return string;
	}

private:
	bool Reserve(size_t numChars) override
	{
		return (numChars < STACK_SIZE);
	}

	char m_charBuffer[STACK_SIZE] = {};
};
