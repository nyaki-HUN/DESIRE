#include "Engine/stdafx.h"
#include "Engine/Core/String/String.h"

#include <errno.h>

const String String::kEmptyString = "";

size_t String::Find(const String& search, size_t startIndex) const
{
	ASSERT(!search.IsEmpty());

	if(startIndex >= size)
	{
		return kInvalidPos;
	}

	const char *foundPtr = strstr(data + startIndex, search.Str());
	return (foundPtr != nullptr) ? foundPtr - data : kInvalidPos;
}

size_t String::Find(char search, size_t startIndex) const
{
	ASSERT(search != '\0');

	if(startIndex >= size)
	{
		return kInvalidPos;
	}

	const char *foundPtr = strchr(data + startIndex, search);
	return (foundPtr != nullptr) ? foundPtr - data : kInvalidPos;
}

size_t String::FindLast(const String& search) const
{
	const char *s = data + size - search.Length();
	while(s >= data)
	{
		if(strncmp(s, search.Str(), search.Length()) == 0)
		{
			return (size_t)(s - data);
		}

		s--;
	}

	return kInvalidPos;
}

size_t String::FindLast(char search) const
{
	const char *s = data + size - 1;
	while(s >= data)
	{
		if(*s == search)
		{
			return (size_t)(s - data);
		}

		s--;
	}

	return kInvalidPos;
}

const char* String::Str() const
{
	return data;
}

size_t String::Length() const
{
	return size;
}

size_t String::LengthUTF8() const
{
	size_t len = 0;
	const char *ch = data;
	while(*ch != '\0')
	{
		if(*ch > 0)
		{
			ch++;
		}
		else
		{
			// Select amongst multi-byte starters
			switch(*ch & 0xF0)
			{
				case 0xE0:	ch += 3; break;
				case 0xF0:	ch += 4; break;
				default:	ch += 2; break;
			}
		}
		len++;
	}

	return len;
}

bool String::IsEmpty() const
{
	return (Length() == 0);
}

int String::IntValue() const
{
	errno = 0;
	char *end = nullptr;
	long value = strtol(data, &end, 10);

	if(end == data || *end != '\0' || errno == ERANGE || value < INT32_MIN || value >= INT32_MAX)
	{
		return INT32_MAX;
	}

	return (int)value;
}

float String::FloatValue() const
{
	errno = 0;
	char *end = nullptr;
	float value = strtof(data, &end);

	if(end == data || *end != '\0' || errno == ERANGE)
	{
		return FLT_MAX;
	}

	return value;
}

int String::Compare(const String& string) const
{
	return strcmp(data, string.data);
}

int String::CompareIgnoreCase(const String& string) const
{
	return StrUtils::Stricmp(data, string.data);
}

bool String::StartsWith(const String& prefix) const
{
	return StartsWith(prefix.Str(), prefix.Length());
}

bool String::StartsWith(const char *prefix, size_t numChars) const
{
	ASSERT(prefix != nullptr);
	return (numChars == 0) ? false : (strncmp(data, prefix, numChars) == 0);
}

bool String::EndsWith(const String& prefix) const
{
	return EndsWith(prefix.Str(), prefix.Length());
}

bool String::EndsWith(const char *suffix, size_t numChars) const
{
	ASSERT(suffix != nullptr);
	return (numChars == 0 || size < numChars) ? false : (strncmp(&data[size - numChars], suffix, numChars) == 0);
}

bool String::EndsWith(char suffix) const
{
	ASSERT(suffix != '\0');
	return (size < 1) ? false : (data[size - 1] == suffix);
}
