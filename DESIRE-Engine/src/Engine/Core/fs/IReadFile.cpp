#include "stdafx.h"
#include "Core/fs/IReadFile.h"

IReadFile::IReadFile(int64_t fileSize, const String& filename)
	: fileSize(fileSize)
	, position(0)
	, filename(filename)
{

}

IReadFile::~IReadFile()
{

}

const String& IReadFile::GetFilename() const
{
	return filename;
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

MemoryBuffer IReadFile::ReadFileContent()
{
	if(IsEof())
	{
		return MemoryBuffer();
	}

	MemoryBuffer buffer = MemoryBuffer((size_t)(fileSize - position));
	const size_t numBytesRead = ReadBuffer(buffer.data, buffer.size);
	ASSERT(numBytesRead == buffer.size);
	return buffer;
}
