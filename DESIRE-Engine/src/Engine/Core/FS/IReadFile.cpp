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

bool IReadFile::ReadString(WritableString& out)
{
	out.Clear();

	uint16_t numChars = 0;
	if(Read(numChars))
	{
		if(numChars == 0)
		{
			return true;
		}

		char* str = out.AsCharBufferWithSize(numChars);
		if(str != nullptr)
		{
			const size_t numBytesRead = ReadBuffer(str, numChars * sizeof(char));
			return numBytesRead == numChars * sizeof(char);
		}
	}

	return false;
}

DynamicString IReadFile::ReadAllAsText()
{
	DynamicString string;

	if(!IsEof())
	{
		const size_t dataSize = static_cast<size_t>(fileSize - position);
		char* data = string.AsCharBufferWithSize(dataSize);
		const size_t numBytesRead = ReadBuffer(data, dataSize);
		ASSERT(numBytesRead == dataSize);
	}

	return string;
}

MemoryBuffer IReadFile::ReadAllAsBinary()
{
	if(!IsEof())
	{
		const size_t dataSize = static_cast<size_t>(fileSize - position);
		MemoryBuffer buffer = MemoryBuffer(dataSize);
		const size_t numBytesRead = ReadBuffer(buffer.ptr.get(), dataSize);
		ASSERT(numBytesRead == dataSize);
		return buffer;
	}

	return MemoryBuffer();
}
