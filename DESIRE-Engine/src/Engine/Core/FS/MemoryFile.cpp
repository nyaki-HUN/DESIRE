#include "Engine/stdafx.h"
#include "Engine/Core/FS/MemoryFile.h"

MemoryFile::MemoryFile(std::unique_ptr<uint8_t[]> spData, size_t size)
	: IReadFile(size)
	, m_spData(std::move(spData))
{
	ASSERT(m_spData != nullptr);
}

MemoryFile::MemoryFile(ReadFilePtr& spFile, size_t sizeToRead)
	: IReadFile(0)
{
	sizeToRead = std::min(sizeToRead, spFile->GetSize());
	m_spData = std::make_unique<uint8_t[]>(sizeToRead);
	const size_t numBytesRead = spFile->ReadBuffer(m_spData.get(), sizeToRead);
	ASSERT(numBytesRead == sizeToRead);
	m_fileSize = static_cast<int64_t>(numBytesRead);
}

bool MemoryFile::Seek(int64_t offset, ESeekOrigin origin)
{
	size_t newPos = SIZE_MAX;
	switch(origin)
	{
		case ESeekOrigin::Begin:	newPos = offset; break;
		case ESeekOrigin::Current:	newPos = m_position + offset; break;
		case ESeekOrigin::End:		newPos = m_fileSize + offset; break;
	}

	if(newPos < m_fileSize)
	{
		m_position = newPos;
		return true;
	}

	return false;
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
