#include "Engine/stdafx.h"
#include "Engine/Core/FS/IReadFile.h"

IReadFile::IReadFile(int64_t fileSize, const String& filename)
	: m_fileSize(fileSize)
	, m_position(0)
	, m_filename(filename)
{
}

IReadFile::~IReadFile()
{
}

const String& IReadFile::GetFilename() const
{
	return m_filename;
}

void IReadFile::ReadBufferAsync(void* buffer, size_t size, std::function<void()> callback)
{
	ASSERT(buffer != nullptr);
	ASSERT(callback != nullptr);

	// Fallback to blocking read
	ReadBuffer(buffer, size);
	callback();
}

bool IReadFile::ReadString(WritableString& string)
{
	string.Clear();

	uint16_t numChars = 0;
	if(Read(numChars))
	{
		if(numChars == 0)
		{
			return true;
		}

		char* pData = string.AsCharBufferWithSize(numChars);
		if(pData != nullptr)
		{
			const size_t numBytesRead = ReadBuffer(pData, numChars * sizeof(char));
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
		const size_t dataSize = static_cast<size_t>(m_fileSize - m_position);
		char* pData = string.AsCharBufferWithSize(dataSize);
		const size_t numBytesRead = ReadBuffer(pData, dataSize);
		ASSERT(numBytesRead == dataSize);
	}

	return string;
}

MemoryBuffer IReadFile::ReadAllAsBinary()
{
	if(!IsEof())
	{
		const size_t dataSize = static_cast<size_t>(m_fileSize - m_position);
		MemoryBuffer buffer = MemoryBuffer(dataSize);
		const size_t numBytesRead = ReadBuffer(buffer.ptr.get(), dataSize);
		ASSERT(numBytesRead == dataSize);
		return buffer;
	}

	return MemoryBuffer();
}
