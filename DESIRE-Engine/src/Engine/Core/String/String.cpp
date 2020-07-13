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
	return std::strtol(data, nullptr, 0);
}

int64_t String::AsInt64() const
{
	return std::strtoll(data, nullptr, 0);
}

uint32_t String::AsUint32() const
{
	return std::strtoul(data, nullptr, 0);
}

uint64_t String::AsUint64() const
{
	return std::strtoull(data, nullptr, 0);
}

float String::AsFloat() const
{
	return std::strtof(data, nullptr);
}

double String::AsDouble() const
{
	return std::strtod(data, nullptr);
}

int String::Compare(const String& string) const
{
	const int rv = memcmp(data, string.data, std::min(size, string.size));
	return (rv != 0) ? rv : static_cast<int>(string.size) - static_cast<int>(size);
}

bool String::Equals(const String& string) const
{
	return (size == string.size) ? (memcmp(data, string.data, size) == 0) : false;
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
