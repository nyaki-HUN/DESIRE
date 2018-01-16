#include "Engine/stdafx.h"
#include "Engine/Core/String.h"
#include "Engine/Core/StrUtils.h"

#include <stdarg.h>

const String String::emptyString;

String::String()
	: data(staticContent)
	, size(0)
{
	staticContent[0] = '\0';
}

String::String(const String& string)
	: data(staticContent)
	, size(0)
{
	InitWithData(string.data, string.size);
}

String::String(String&& string)
{
	if(string.data == string.staticContent)
	{
		memcpy(staticContent, string.staticContent, kStackSize);
		data = staticContent;
		size = string.size;
	}
	else
	{
		data = string.data;
		size = string.size;
		string.data = string.staticContent;
		string.size = 0;
	}
}

String::String(const char *s)
	: data(staticContent)
	, size(0)
{
	ASSERT(s != nullptr);
	InitWithData(s, strlen(s));
}

String::String(const char *i_data, size_t size)
	: data(staticContent)
	, size(0)
{
	ASSERT(i_data != nullptr);
	InitWithData(i_data, size);
}

String::~String()
{
	if(data != staticContent)
	{
		delete[] data;
	}
}

void String::ToLower()
{
	StrUtils::ToLower(data);
}

void String::ToUpper()
{
	StrUtils::ToUpper(data);
}

void String::Trim()
{
	// Remove from end
	while(size > 0 && StrUtils::IsSpace(data[size - 1]))
	{
		size--;
	}
	data[size] = '\0';

	// Remove from beginning
	const char *newData = StrUtils::SkipSpaces(data);
	if(newData != data)
	{
		size -= newData - data;
		memmove(data, newData, size + 1);
	}
}

void String::Append(const String& string)
{
	Append(string.c_str(), string.size);
}

void String::Append(const char *s)
{
	Append(s, strlen(s));
}

void String::Append(const char *s, size_t numChars)
{
	ASSERT(s != nullptr);

	const size_t oldSize = size;
	EnsureSize(size + numChars, true);
	memcpy(data + oldSize, s, numChars);
	data[size] = '\0';
}

void String::Insert(size_t startIndex, const String& string)
{
	Insert(startIndex, string.c_str(), string.size);
}

void String::Insert(size_t startIndex, const char *s)
{
	ASSERT(s != nullptr);
	Insert(startIndex, s, strlen(s));
}

void String::Insert(size_t startIndex, const char *s, size_t numChars)
{
	ASSERT(s != nullptr);

	if(startIndex > size)
	{
		return;
	}

	const size_t oldSize = size;
	EnsureSize(size + numChars, true);
	memmove(data + startIndex + numChars, data + startIndex, oldSize - startIndex + 1);
	memcpy(data + startIndex, s, numChars);
}

void String::Remove(size_t startIndex, size_t numChars)
{
	if(startIndex >= size)
	{
		return;
	}

	if(startIndex + numChars > size)
	{
		numChars = size - startIndex;
	}
	size -= startIndex + numChars;
	memmove(data + startIndex, data + startIndex + numChars, size + 1);
}

void String::RemoveFromEnd(size_t numChars)
{
	size -= (numChars <= size) ? numChars : size;
	data[size] = '\0';
}

size_t String::Find(const String& search, size_t startIndex) const
{
	return Find(search.data, startIndex);
}

size_t String::Find(const char *search, size_t startIndex) const
{
	ASSERT(search != nullptr);
	ASSERT(search[0] != '\0');

	if(startIndex >= size)
	{
		return INVALID_POS;
	}

	const char *foundPtr = strstr(data + startIndex, search);
	return (foundPtr != nullptr) ? foundPtr - data : INVALID_POS;
}

size_t String::Find(char search, size_t startIndex) const
{
	ASSERT(search != '\0');

	if(startIndex >= size)
	{
		return INVALID_POS;
	}

	const char *foundPtr = strchr(data + startIndex, search);
	return (foundPtr != nullptr) ? foundPtr - data : INVALID_POS;
}

size_t String::FindLast(const String& search) const
{
	return FindLast(search.data);
}

size_t String::FindLast(const char *search) const
{
	ASSERT(search != nullptr);

	const size_t searchLen = strlen(search);
	const char *s = data + size - searchLen;
	while(s >= data)
	{
		if(strncmp(s, search, searchLen) == 0)
		{
			return (size_t)(s - data);
		}
		s--;
	}
	return INVALID_POS;
}

size_t String::FindLast(char search) const
{
	const char *foundPtr = strrchr(data, search);
	return (foundPtr != nullptr) ? foundPtr - data : INVALID_POS;
}

String String::SubString(size_t startIndex, size_t numChars) const
{
	if(startIndex >= size)
	{
		return String();
	}

	return String(&data[startIndex], std::min(numChars, size - startIndex));
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

const char* String::c_str() const
{
	return data;
}

int String::IntValue() const
{
	return StrUtils::ParseInt(data);
}

float String::FloatValue() const
{
	return StrUtils::ParseFloat(data);
}

String& String::operator =(const String& string)
{
	ASSERT(this != &string);	// It's not allowed to copy from ourself

	if(data != staticContent)
	{
		delete[] data;
		data = staticContent;
	}
	InitWithData(string.data, string.size);
	return *this;
}

String& String::operator =(String&& string)
{
	ASSERT(this != &string);	// It's not allowed to copy from ourself

	if(data != staticContent)
	{
		delete[] data;
	}

	if(string.data == string.staticContent)
	{
		memcpy(staticContent, string.staticContent, kStackSize);
		data = staticContent;
		size = string.size;
	}
	else
	{
		data = string.data;
		size = string.size;
		string.data = string.staticContent;
		string.size = 0;
	}
	return *this;
}

String& String::operator =(const char *s)
{
	ASSERT(data != s);			// It's not allowed to copy from ourself

	if(data != staticContent)
	{
		delete[] data;
		data = staticContent;
	}
	InitWithData(s, strlen(s));
	return *this;
}

String& String::operator +=(int32_t number)
{
	char s[10 + 2];
	snprintf(s, sizeof(s), "%d", number);
	Append(s);
	return *this;
}

String& String::operator +=(uint32_t number)
{
	char s[10 + 2];
	snprintf(s, sizeof(s), "%u", number);
	Append(s);
	return *this;
}

String& String::operator +=(int64_t number)
{
	char s[20 + 2];
	snprintf(s, sizeof(s), "%lld", number);
	Append(s);
	return *this;
}

String& String::operator +=(uint64_t number)
{
	char s[20 + 2];
	snprintf(s, sizeof(s), "%llu", number);
	Append(s);
	return *this;
}

String& String::operator +=(float number)
{
	char s[32];
	snprintf(s, sizeof(s), "%.3f", number);
	Append(s);
	return *this;
}

String& String::operator +=(bool b)
{
	Append(b ? "true" : "false");
	return *this;
}

int String::Compare(const String& string) const
{
	return strcmp(data, string.data);
}

int String::Compare(const char *s) const
{
	return strcmp(data, s);
}

int String::CompareIgnoreCase(const String& string) const
{
	return StrUtils::Stricmp(data, string.data);
}

int String::CompareIgnoreCase(const char *s) const
{
	return StrUtils::Stricmp(data, s);
}

bool String::StartsWith(const String& prefix) const
{
	ASSERT(prefix.size != 0);
	return (strncmp(data, prefix.c_str(), prefix.size) == 0);
}

bool String::StartsWith(const char *prefix) const
{
	ASSERT(prefix != nullptr && strlen(prefix) != 0);
	return (strncmp(data, prefix, strlen(prefix)) == 0);
}

bool String::EndsWith(const String& suffix) const
{
	ASSERT(suffix.size != 0);
	return (size < suffix.size) ? false : (strcmp(&data[size - suffix.size], suffix.c_str()) == 0);
}

bool String::EndsWith(const char *suffix) const
{
	ASSERT(suffix != nullptr && suffix[0] != '\0');
	const size_t suffixLen = strlen(suffix);
	return (size < suffixLen) ? false : (strcmp(&data[size - suffixLen], suffix) == 0);
}

bool String::EndsWith(char suffix) const
{
	ASSERT(suffix != '\0');
	return (size < 1) ? false : (data[size - 1] == suffix);
}

String String::CreateFormattedString(const char *format, ...)
{
	ASSERT(format != nullptr);
	char str[1024];

	va_list args;
	va_start(args, format);
	const int size = vsnprintf(str, sizeof(str), format, args);
	ASSERT(size < sizeof(str));
	va_end(args);

	return String(str, (size_t)size);
}

String String::CreateFromInt(int num)
{
	String string;
	// It is safe to use kStackSize here as data is always at least that big
	snprintf(string.data, kStackSize, "%d", num);
	return string;
}

void String::Replace_Internal(const char *search, const char *replaceTo, bool all)
{
	ASSERT(search != nullptr);
	ASSERT(replaceTo != nullptr);

	const size_t searchLen = strlen(search);
	const size_t replaceToLen = strlen(replaceTo);

	char *dataTmp = data;
	String newString;
	for(;;)
	{
		char *foundPtr = strstr(dataTmp, search);
		if(foundPtr == nullptr)
		{
			break;
		}
		*foundPtr = '\0';
		newString.Append(dataTmp, static_cast<size_t>(foundPtr - dataTmp));
		newString.Append(replaceTo, replaceToLen);
		dataTmp = foundPtr + searchLen;
		if(!all)
		{
			break;
		}
	}
	// Add remaining part
	newString += dataTmp;

	// Move back the content from newString
	*this = std::move(newString);
}

void String::EnsureSize(size_t newSize, bool keepOld)
{
	if(newSize >= kStackSize)
	{
		char *oldData = data;
		data = new char[newSize + 1];
		if(keepOld)
		{
			strcpy_s(data, newSize + 1, oldData);
		}

		if(oldData != staticContent)
		{
			delete[] oldData;
		}
	}

	size = newSize;
}

void String::InitWithData(const char *i_data, size_t newSize)
{
	EnsureSize(newSize, false);
	memcpy(data, i_data, size);
	data[size] = '\0';
}
