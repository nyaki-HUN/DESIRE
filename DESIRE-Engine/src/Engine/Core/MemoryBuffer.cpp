#include "Engine/stdafx.h"
#include "Engine/Core/MemoryBuffer.h"

MemoryBuffer::MemoryBuffer(std::unique_ptr<uint8_t[]> data, size_t size)
	: ptr(std::move(data))
	, size(size)
{
}

MemoryBuffer::MemoryBuffer(size_t size)
	: MemoryBuffer(std::make_unique<uint8_t[]>(size), size)
{
}

String MemoryBuffer::AsString() const
{
	return (size != 0) ? String(reinterpret_cast<const char*>(ptr.get()), size - 1) : "";
}

MemoryBuffer MemoryBuffer::CreateFromDataCopy(const void* dataToCopy, size_t size)
{
	MemoryBuffer buffer(size);
	memcpy(buffer.ptr.get(), dataToCopy, size);
	return buffer;
}
