#include "Engine/stdafx.h"
#include "Engine/Core/MemoryBuffer.h"

MemoryBuffer::MemoryBuffer(std::unique_ptr<uint8_t[]> spData, size_t size)
	: m_spData(std::move(spData))
	, m_size(size)
{
}

MemoryBuffer::MemoryBuffer(size_t size)
	: m_spData(std::make_unique<uint8_t[]>(size))
	, m_size(size)
{
}

uint8_t* MemoryBuffer::GetData() const
{
	return m_spData.get();
}

size_t MemoryBuffer::GetSize() const
{
	return m_size;
}

MemoryBuffer MemoryBuffer::CreateFromDataCopy(const void* pDataToCopy, size_t size)
{
	MemoryBuffer buffer(size);
	memcpy(buffer.GetData(), pDataToCopy, size);
	return buffer;
}
