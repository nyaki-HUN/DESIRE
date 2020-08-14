#include "Engine/stdafx.h"
#include "Engine/Core/String/DynamicString.h"

#include "Engine/Core/Memory/MemorySystem.h"

static char emptyData[1] = { '\0' };

DynamicString::DynamicString()
{
	pData = emptyData;
}

DynamicString::DynamicString(const char* str, size_t size)
{
	Set(str, size);
}

DynamicString::DynamicString(DynamicString&& string)
{
	pData = string.pData;
	size = string.size;
	preallocatedSize = string.preallocatedSize;

	string.pData = emptyData;
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

	pData = string.pData;
	size = string.size;
	preallocatedSize = string.preallocatedSize;

	string.pData = emptyData;
	string.size = 0;
	string.preallocatedSize = 0;

	return *this;
}

DynamicString DynamicString::SubString(size_t pos, size_t numChars) const
{
	if(pos < size)
	{
		return DynamicString(&pData[pos], std::min(numChars, size - pos));
	}

	return DynamicString();
}

void DynamicString::Reset()
{
	if(pData != emptyData)
	{
		MemorySystem::Free(pData);
		pData = emptyData;
		size = 0;
		preallocatedSize = 0;
	}
}

bool DynamicString::Reserve(size_t numChars)
{
	if(preallocatedSize < numChars + 1)
	{
		preallocatedSize = numChars + 1;

		if(pData != emptyData)
		{
			pData = static_cast<char*>(MemorySystem::Realloc(pData, preallocatedSize * sizeof(char)));
		}
		else
		{
			pData = static_cast<char*>(MemorySystem::Alloc(preallocatedSize * sizeof(char)));
		}
	}

	return true;
}
