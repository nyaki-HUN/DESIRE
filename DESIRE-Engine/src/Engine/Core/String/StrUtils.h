#pragma once

#include <functional>

class StrUtils
{
public:
	// Parses the C string str, interpreting its content as an integral number. Returns INT32_MAX on error.
	static int ParseInt(const char *str);

	// Parses the C string str, interpreting its content as a floating point number. Returns FLT_MAX on error.
	static float ParseFloat(const char *str);

	// Duplicates a string using malloc. Doesn't allocate if 'str' is nullptr
	static char* Duplicate(const char *str);

	// Case-insensitive comparison of strings
	static int Stricmp(const char *str1, const char *str2);
	static int Strnicmp(const char *str1, const char *str2, size_t n);

	static void ToLower(char *str);
	static void ToUpper(char *str);

	static void Tokenize(const char *str, char separator, std::function<void(const char*, size_t)> funcToCallWithTokens);

	inline static bool IsSpace(char c)
	{
		return (c == ' ' || c == '\t' || c == '\n' || c == '\r');
	}

	inline static const char* SkipSpaces(const char *str)
	{
		while(*str != '\0' && StrUtils::IsSpace(*str))
		{
			++str;
		}
		return str;
	}
};
