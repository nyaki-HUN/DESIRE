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

size_t IReadFile::ReadString(WritableString& out)
{
	uint16_t len;
	if(!Read(len) || len == 0)
	{
		out.Clear();
		return 0;
	}

	ASSERT(false);
	// TODO
	return 0;
}

MemoryBuffer IReadFile::ReadAllContents()
{
	if(!IsEof())
	{
		const size_t dataSize = static_cast<size_t>(fileSize - position);
		MemoryBuffer buffer = MemoryBuffer(dataSize + 1);
		const size_t numBytesRead = ReadBuffer(buffer.ptr.get(), dataSize);
		ASSERT(numBytesRead == dataSize);
		buffer.ptr[numBytesRead] = '\0';
		return buffer;
	}

	return MemoryBuffer();
}
