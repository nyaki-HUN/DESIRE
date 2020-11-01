#include "Engine/stdafx.h"
#include "Engine/Core/MemoryBuffer.h"

MemoryBuffer::MemoryBuffer(std::unique_ptr<uint8_t[]> spData, size_t size)
	: ptr(std::move(spData))
	, size(size)
{
}

MemoryBuffer::MemoryBuffer(size_t size)
	: ptr(std::make_unique<uint8_t[]>(size))
	, size(size)
{
}

MemoryBuffer MemoryBuffer::CreateFromDataCopy(const void* pDataToCopy, size_t size)
{
	MemoryBuffer buffer(size);
	memcpy(buffer.ptr.get(), pDataToCopy, size);
	return buffer;
}
