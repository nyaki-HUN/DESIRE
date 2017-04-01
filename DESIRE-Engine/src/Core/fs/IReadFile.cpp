#include "stdafx.h"
#include "Core/fs/IReadFile.h"

IReadFile::IReadFile(int64_t fileSize)
	: fileSize(fileSize)
	, position(0)
{

}

IReadFile::~IReadFile()
{

}

size_t IReadFile::ReadString(char **str)
{
	ASSERT(str != nullptr);

	uint16_t len;
	if(!Read(len) || len == 0)
	{
		*str = nullptr;
		return 0;
	}

	*str = (char*)malloc(len);
	return ReadBuffer(*str, len);
}

SMemoryBuffer IReadFile::ReadFileContent()
{
	if(IsEof())
	{
		return SMemoryBuffer();
	}


	SMemoryBuffer buffer = SMemoryBuffer((size_t)(fileSize - position));
	size_t numBytesRead = ReadBuffer(buffer.data, buffer.size);
	ASSERT(numBytesRead == buffer.size);
	return buffer;
}
