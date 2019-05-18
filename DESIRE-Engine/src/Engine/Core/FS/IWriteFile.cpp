#include "Engine/stdafx.h"
#include "Engine/Core/FS/IWriteFile.h"

IWriteFile::IWriteFile()
{

}

IWriteFile::~IWriteFile()
{

}

bool IWriteFile::WriteString(const char *str, size_t len)
{
	if(str == nullptr)
	{
		Write<uint16_t>(0);
		return true;
	}

	// Calculate the length if needed
	if(len == SIZE_MAX)
	{
		len = strlen(str);
	}

	if(len > UINT16_MAX)
	{
		ASSERT(false && "Unupported string length");
		return false;
	}

	const size_t numBytesWritten = Write((uint16_t)len) + WriteBuffer(str, len);
	return (numBytesWritten == sizeof(uint16_t) + len);
}
