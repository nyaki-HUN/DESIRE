#include "Engine/stdafx.h"
#include "Engine/Utils/StrUtils.h"

char* StrUtils::Duplicate(const char* str)
{
	if(str == nullptr)
	{
		return nullptr;
	}

	const size_t len = strlen(str);
	char* newStr = (char*)malloc(len + 1);
	if(newStr != nullptr)
	{
		memcpy(newStr, str, len + 1);
	}
	return newStr;
}

int StrUtils::Stricmp(const char* str1, const char* str2)
{
#if DESIRE_PLATFORM_WINDOWS
	return _stricmp(str1, str2);
#else
	return strcasecmp(str1, str2);
#endif
}

int StrUtils::Strnicmp(const char* str1, const char* str2, size_t n)
{
#if DESIRE_PLATFORM_WINDOWS
	return _strnicmp(str1, str2, n);
#else
	return strncasecmp(str1, str2, n);
#endif
}

void StrUtils::Tokenize(const char* str, char separator, std::function<void(const char*, size_t)> funcToCallWithTokens)
{
	const char* tokenStart = str;
	const char* tokenEnd = strchr(tokenStart, separator);
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
