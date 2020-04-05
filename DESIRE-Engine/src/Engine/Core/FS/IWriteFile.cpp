#include "Engine/stdafx.h"
#include "Engine/Core/FS/IWriteFile.h"

#include "Engine/Core/String/String.h"

IWriteFile::IWriteFile()
{
}

IWriteFile::~IWriteFile()
{
}

bool IWriteFile::WriteString(const String& string)
{
	const size_t numChars = string.Length();
	if(numChars > UINT16_MAX)
	{
		ASSERT(false && "Unupported string length");
		return false;
	}

	const size_t numBytesWritten = Write(static_cast<uint16_t>(numChars)) + WriteBuffer(string.Str(), numChars);
	return (numBytesWritten == sizeof(uint16_t) + numChars);
}
