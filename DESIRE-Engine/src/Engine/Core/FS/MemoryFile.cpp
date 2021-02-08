#include "Engine/stdafx.h"
#include "Engine/Core/FS/MemoryFile.h"

MemoryFile::MemoryFile(std::unique_ptr<uint8_t[]> spData, int64_t size)
	: IReadFile(size)
	, m_spData(std::move(spData))
{
	ASSERT(m_fileSize >= 0);
	ASSERT(m_spData != nullptr);
}

MemoryFile::MemoryFile(ReadFilePtr& spFile, int64_t size)
	: IReadFile(0)
{
	if(size < 0)
	{
		size = spFile->GetSize();
	}

	ASSERT(size >= 0 && size < SIZE_MAX);
	const size_t sizeToRead = static_cast<size_t>(size);
	m_spData = std::make_unique<uint8_t[]>(sizeToRead);
	const size_t numBytesRead = spFile->ReadBuffer(m_spData.get(), sizeToRead);
	ASSERT(numBytesRead == sizeToRead);
	m_fileSize = static_cast<int64_t>(numBytesRead);
}

bool MemoryFile::Seek(int64_t offset, ESeekOrigin origin)
{
	int64_t newPos = -1;
	switch(origin)
	{
		case ESeekOrigin::Begin:	newPos = offset; break;
		case ESeekOrigin::Current:	newPos = m_position + offset; break;
		case ESeekOrigin::End:		newPos = m_fileSize + offset; break;
	}

	if(newPos < 0 || newPos > m_fileSize)
	{
		return false;
	}

	m_position = newPos;
	return true;
}

void MemoryFile::ReadBufferAsync(void* pBuffer, size_t size, std::function<void()> callback)
{
	ASSERT(pBuffer != nullptr);
	ASSERT(callback != nullptr);

	// Data is already in memory so we can do blocking read (which will just copy it)
	ReadBuffer(pBuffer, size);
	callback();
}

size_t MemoryFile::ReadBuffer(void* pBuffer, size_t size)
{
	const size_t remainingSize = static_cast<size_t>(m_fileSize - m_position);
	size = std::min(size, remainingSize);

	std::memcpy(pBuffer, m_spData.get() + m_position, size);
	m_position += size;
	return size;
}
