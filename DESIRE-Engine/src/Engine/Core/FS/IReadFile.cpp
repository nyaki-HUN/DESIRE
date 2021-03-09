#include "Engine/stdafx.h"
#include "Engine/Core/FS/IReadFile.h"

IReadFile::IReadFile(size_t fileSize, const String& filename)
	: m_fileSize(fileSize)
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

size_t IReadFile::GetSize() const
{
	return m_fileSize;
}

size_t IReadFile::Tell() const
{
	return m_position;
}

bool IReadFile::IsEof() const
{
	return m_position >= m_fileSize;
}

void IReadFile::ReadBufferAsync(void* pBuffer, size_t size, std::function<void()> callback)
{
	ASSERT(pBuffer != nullptr);
	ASSERT(callback != nullptr);

	// Fallback to blocking read
	ReadBuffer(pBuffer, size);
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
		if(pData)
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
		const size_t dataSize = m_fileSize - m_position;
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
		const size_t dataSize = m_fileSize - m_position;
		MemoryBuffer buffer = MemoryBuffer(dataSize);
		const size_t numBytesRead = ReadBuffer(buffer.GetData(), dataSize);
		ASSERT(numBytesRead == dataSize);
		return buffer;
	}

	return MemoryBuffer();
}
