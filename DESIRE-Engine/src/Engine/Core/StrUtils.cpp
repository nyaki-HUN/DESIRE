#include "stdafx.h"
#include "Core/StrUtils.h"

#include <errno.h>
#include <cctype>	// for std::tolower() and std::toupper()

int StrUtils::ParseInt(const char *str)
{
	errno = 0;
	char *end = nullptr;
	long value = strtol(str, &end, 10);

	if(end == str || *end != '\0' || errno == ERANGE || value < INT32_MIN || value >= INT32_MAX)
	{
		return INT32_MAX;
	}

	return (int)value;
}

float StrUtils::ParseFloat(const char *str)
{
	errno = 0;
	char *end = nullptr;
	float value = strtof(str, &end);

	if(end == str || *end != '\0' || errno == ERANGE)
	{
		return FLT_MAX;
	}

	return value;
}

char* StrUtils::Duplicate(const char *str)
{
	if(str == nullptr)
	{
		return nullptr;
	}

	const size_t len = strlen(str);
	char *newStr = (char*)malloc(len + 1);
	memcpy(newStr, str, len + 1);
	return newStr;
}

int StrUtils::Stricmp(const char *str1, const char *str2)
{
#if defined(DESIRE_PLATFORM_WINDOWS)
	return _stricmp(str1, str2);
#else
	return strcasecmp(str1, str2);
#endif
}

int StrUtils::Strnicmp(const char *str1, const char *str2, size_t n)
{
#if defined(DESIRE_PLATFORM_WINDOWS)
	return _strnicmp(str1, str2, n);
#else
	return strncasecmp(str1, str2, n);
#endif
}

void StrUtils::ToLower(char *str)
{
	char *ch = str;
	while(*ch != '\0')
	{
		*ch = (char)std::tolower(*ch);
		ch++;
	}
}

void StrUtils::ToUpper(char *str)
{
	char *ch = str;
	while(*ch != '\0')
	{
		*ch = (char)std::toupper(*ch);
		ch++;
	}
}

void StrUtils::Tokenize(const char *str, char separator, std::function<void(const char*, size_t)> funcToCallWithTokens)
{
	const char *tokenStart = str;
	const char *tokenEnd = strchr(tokenStart, separator);
	while(tokenEnd != nullptr)
	{
		const size_t size = (size_t)(tokenEnd - tokenStart);
		if(size != 0)
		{
			funcToCallWithTokens(tokenStart, size);
		}

		tokenStart = tokenEnd + 1;
		tokenEnd = strchr(tokenStart, separator);
	}

	funcToCallWithTokens(tokenStart, strlen(tokenStart));
}
