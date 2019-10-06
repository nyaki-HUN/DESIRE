#include "Engine/stdafx.h"
#include "Engine/Core/FS/IReadFile.h"

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

void IReadFile::ReadBufferAsync(void* buffer, size_t size, std::function<void()> callback)
{
	ASSERT(buffer != nullptr);
	ASSERT(callback != nullptr);

	// Fallback to blocking read
	ReadBuffer(buffer, size);
	callback();
}

size_t IReadFile::ReadString(char** str)
{
	ASSERT(str != nullptr);

	uint16_t len;
	if(!Read(len) || len == 0)
	{
		*str = nullptr;
		return 0;
	}

	*str = static_cast<char*>(malloc(len));
	return ReadBuffer(*str, len);
}

MemoryBuffer IReadFile::ReadFileContent()
{
	if(IsEof())
	{
		return MemoryBuffer();
	}

	const size_t dataSize = static_cast<size_t>(fileSize - position);
	MemoryBuffer buffer = MemoryBuffer(dataSize + 1);
	const size_t numBytesRead = ReadBuffer(buffer.data, dataSize);
	ASSERT(numBytesRead == dataSize);
	buffer.data[numBytesRead] = '\0';
	return buffer;
}
