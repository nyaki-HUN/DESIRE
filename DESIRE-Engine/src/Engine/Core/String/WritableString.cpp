#include "Engine/stdafx.h"
#include "Engine/Core/String/WritableString.h"
#include "Engine/Core/String/StrUtils.h"

WritableString::WritableString()
{

}

void WritableString::ToLower()
{
	StrUtils::ToLower(data);
}

void WritableString::ToUpper()
{
	StrUtils::ToUpper(data);
}

void WritableString::Trim()
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

void WritableString::Insert(size_t startIndex, const char *str, size_t numChars)
{
	ASSERT(str != nullptr);

	if(startIndex > size)
	{
		return;
	}

	const bool hasEnoughSize = Reserve(size + numChars);
	ASSERT(hasEnoughSize);
	if(hasEnoughSize)
	{
		memmove(data + startIndex + numChars, data + startIndex, size - startIndex + 1);
		memcpy(data + startIndex, str, numChars);
		size += numChars;
	}
}

void WritableString::RemoveFrom(size_t startIndex, size_t numChars)
{
	if(startIndex >= size)
	{
		return;
	}

	numChars = std::min(numChars, size - startIndex);

	memmove(data + startIndex, data + startIndex + numChars, size - startIndex - numChars + 1);
	size -= numChars;
}

void WritableString::RemoveFromEnd(size_t numChars)
{
	size -= (numChars <= size) ? numChars : size;
	data[size] = '\0';
}

void WritableString::Replace(const String& search, const String& replaceTo)
{
	const size_t firstPos = Find(search);
	if(firstPos == kInvalidPos)
	{
		return;
	}

	const int64_t numCharsToMove = replaceTo.Length() - search.Length();
	if(numCharsToMove > 0)
	{
		const bool hasEnoughSize = Reserve(size + numCharsToMove);
		ASSERT(hasEnoughSize);
		if(!hasEnoughSize)
		{
			return;
		}
	}

	Replace_Internal(firstPos, search.Length(), replaceTo);
}

void WritableString::ReplaceAll(const String& search, const String& replaceTo)
{
	const size_t firstPos = Find(search);
	if(firstPos == kInvalidPos)
	{
		return;
	}

	const int64_t numCharsToMove = replaceTo.Length() - search.Length();
	if(numCharsToMove > 0)
	{
		size_t count = 0;
		size_t pos = firstPos;
		do
		{
			count++;
			pos = Find(search, pos + search.Length());
		} while(pos != kInvalidPos);

		const bool hasEnoughSize = Reserve(size + count * numCharsToMove);
		ASSERT(hasEnoughSize);
		if(!hasEnoughSize)
		{
			return;
		}
	}

	size_t pos = firstPos;
	do
	{
		Replace_Internal(pos, search.Length(), replaceTo);
		pos = Find(search, pos + replaceTo.Length());
	} while(pos != kInvalidPos);
}

void WritableString::ReplaceAllChar(char search, char replaceTo)
{
	ASSERT(search != '\0');

	char *dataTmp = data;
	for(;;)
	{
		char *foundPtr = strchr(dataTmp, search);
		if(foundPtr == nullptr)
		{
			break;
		}

		*foundPtr = replaceTo;
		dataTmp = foundPtr + 1;
	}
}

void WritableString::Append(const char *str, size_t numChars)
{
	ASSERT(str != nullptr);

	const bool hasEnoughSize = Reserve(size + numChars);
	ASSERT(hasEnoughSize);
	if(hasEnoughSize)
	{
		memcpy(data + size, str, numChars);
		size += numChars;
		data[size] = '\0';
	}
}

WritableString& WritableString::operator +=(int32_t number)
{
	char str[10 + 2];
	const int len = snprintf(str, sizeof(str), "%d", number);
	if(len > 0)
	{
		Append(str, len);
	}
	return *this;
}

WritableString& WritableString::operator +=(uint32_t number)
{
	char str[10 + 2];
	const int len = snprintf(str, sizeof(str), "%u", number);
	if(len > 0)
	{
		Append(str, len);
	}
	return *this;
}

WritableString& WritableString::operator +=(int64_t number)
{
	char str[20 + 2];
	const int len = snprintf(str, sizeof(str), "%lld", number);
	if(len > 0)
	{
		Append(str, len);
	}
	return *this;
}

WritableString& WritableString::operator +=(uint64_t number)
{
	char str[20 + 2];
	const int len = snprintf(str, sizeof(str), "%llu", number);
	if(len > 0)
	{
		Append(str, len);
	}
	return *this;
}

WritableString& WritableString::operator +=(float number)
{
	char str[32];
	const int len = snprintf(str, sizeof(str), "%.3f", number);
	if(len > 0)
	{
		Append(str, len);
	}
	return *this;
}

void WritableString::Replace_Internal(size_t pos, size_t searchLen, const String& replaceTo)
{
	memmove(data + pos + replaceTo.Length(), data + pos + searchLen, size - pos - searchLen + 1);
	size += replaceTo.Length() - searchLen;

	memcpy(data + pos, replaceTo.Str(), replaceTo.Length());
}

void WritableString::InitWithData(const char *newData, size_t newSize)
{
	ASSERT(newData != nullptr);

	const bool hasEnoughSize = Reserve(newSize);
	ASSERT(hasEnoughSize);
	if(hasEnoughSize)
	{
		memcpy(data, newData, newSize);
		size = newSize;
		data[size] = '\0';
	}
}
