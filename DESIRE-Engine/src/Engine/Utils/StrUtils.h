#pragma once

#include <functional>

class StrUtils
{
public:
	// Duplicates a string using malloc. Doesn't allocate if 'str' is nullptr
	static char* Duplicate(const char *str);

	// Case-insensitive comparison of strings
	static int Stricmp(const char *str1, const char *str2);
	static int Strnicmp(const char *str1, const char *str2, size_t n);

	static void Tokenize(const char *str, char separator, std::function<void(const char*, size_t)> funcToCallWithTokens);
};
