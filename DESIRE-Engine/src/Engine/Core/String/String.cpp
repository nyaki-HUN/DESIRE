#include "Engine/stdafx.h"
#include "Engine/Core/String/String.h"

const String String::kEmptyString = "";

String::String(const char* str, size_t numChars)
	: data(const_cast<char*>(str))
	, size(numChars)
{
	ASSERT(data != nullptr);
	ASSERT(data[size] == '\0');
}

size_t String::Find(const String& search, size_t startIndex) const
{
	if(search.size == 0 || startIndex >= size)
	{
		return kInvalidPos;
	}

	const char* foundPtr = strstr(data + startIndex, search.data);
	return (foundPtr != nullptr) ? foundPtr - data : kInvalidPos;
}

size_t String::Find(char search, size_t startIndex) const
{
	if(startIndex >= size || search == '\0')
	{
		return kInvalidPos;
	}

	const char* foundPtr = strchr(data + startIndex, search);
	return (foundPtr != nullptr) ? foundPtr - data : kInvalidPos;
}

size_t String::FindLast(const String& search) const
{
	const char* s = data + size - search.size;
	while(s >= data)
	{
		if(memcmp(s, search.data, search.size) == 0)
		{
			return static_cast<size_t>(s - data);
		}

		s--;
	}

	return kInvalidPos;
}

size_t String::FindLast(char search) const
{
	const char* s = data + size - 1;
	while(s >= data)
	{
		if(*s == search)
		{
			return static_cast<size_t>(s - data);
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
	const char* ch = data;
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
	return (size == 0);
}

int32_t String::AsInt32() const
{
	char* end = nullptr;
	const int32_t value = strtol(data, &end, 10);
	if(end != data + size)
	{
		return INT32_MAX;
	}

	return value;
}

int64_t String::AsInt64() const
{
	char* end = nullptr;
	const int64_t value = strtoll(data, &end, 0);
	if(end != data + size)
	{
		return INT64_MAX;
	}

	return value;
}

uint32_t String::AsUint32() const
{
	char* end = nullptr;
	const uint32_t value = strtoul(data, &end, 0);
	if(end != data + size)
	{
		return UINT32_MAX;
	}

	return value;
}

uint64_t String::AsUint64() const
{
	char* end = nullptr;
	const uint64_t value = strtoull(data, &end, 0);
	if(end != data + size)
	{
		return UINT64_MAX;
	}

	return value;
}

float String::AsFloat() const
{
	char* end = nullptr;
	const float value = strtof(data, &end);
	if(end != data + size)
	{
		return FLT_MAX;
	}

	return value;
}

double String::AsDouble() const
{
	char* end = nullptr;
	const double value = strtod(data, &end);
	if(end != data + size)
	{
		return DBL_MAX;
	}

	return value;
}

int String::Compare(const String& string) const
{
	const int rv = memcmp(data, string.data, std::min(size, string.size));
	return (rv != 0) ? rv : static_cast<int>(string.size) - static_cast<int>(size);
}

int String::CompareIgnoreCase(const String& string) const
{
#if DESIRE_PLATFORM_WINDOWS
	return _stricmp(data, string.data);
#else
	return strcasecmp(data, string.data);
#endif
}

bool String::Equals(const String& string) const
{
	return (size == string.size) ? (memcmp(data, string.data, size) == 0) : false;
}

bool String::EqualsIgnoreCase(const String& string) const
{
	return (size == string.size) ? (CompareIgnoreCase(string) == 0) : false;
}

bool String::StartsWith(const String& prefix) const
{
	if(size >= prefix.size && prefix.size != 0)
	{
		return (memcmp(data, prefix.data, prefix.size) == 0);
	}

	return false;
}

bool String::StartsWith(char prefix) const
{
	return (data[0] == prefix);
}

bool String::EndsWith(const String& suffix) const
{
	if(size >= suffix.size && suffix.size != 0)
	{
		return (memcmp(&data[size - suffix.size], suffix.data, suffix.size) == 0);
	}

	return false;
}

bool String::EndsWith(char suffix) const
{
	ASSERT(suffix != '\0');
	return (size < 1) ? false : (data[size - 1] == suffix);
}
