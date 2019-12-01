#include "Engine/stdafx.h"
#include "Engine/Core/String/DynamicString.h"
#include "Engine/Core/Memory/MemorySystem.h"

static char emptyData[1] = { '\0' };

DynamicString::DynamicString()
{
	data = emptyData;
}

DynamicString::DynamicString(const char* str, size_t size)
{
	Set(str, size);
}

DynamicString::DynamicString(DynamicString&& string)
{
	data = string.data;
	size = string.size;
	preallocatedSize = string.preallocatedSize;

	string.data = emptyData;
	string.size = 0;
	string.preallocatedSize = 0;
}

DynamicString::~DynamicString()
{
	Reset();
}

DynamicString& DynamicString::operator =(DynamicString&& string)
{
	ASSERT(this != &string);	// It's not allowed to copy from ourself

	Reset();

	data = string.data;
	size = string.size;
	preallocatedSize = string.preallocatedSize;

	string.data = emptyData;
	string.size = 0;
	string.preallocatedSize = 0;

	return *this;
}

DynamicString DynamicString::SubString(size_t startIndex, size_t numChars) const
{
	if(startIndex >= size)
	{
		return DynamicString();
	}

	return DynamicString(&data[startIndex], std::min(numChars, size - startIndex));
}

void DynamicString::Reset()
{
	if(data != emptyData)
	{
		MemorySystem::Free(data);
		data = emptyData;
		size = 0;
		preallocatedSize = 0;
	}
}

bool DynamicString::Reserve(size_t numChars)
{
	if(preallocatedSize < numChars + 1)
	{
		preallocatedSize = numChars + 1;

		if(data != emptyData)
		{
			data = static_cast<char*>(MemorySystem::Realloc(data, preallocatedSize * sizeof(char)));
		}
		else
		{
			data = static_cast<char*>(MemorySystem::Alloc(preallocatedSize * sizeof(char)));
		}
	}

	return true;
}
