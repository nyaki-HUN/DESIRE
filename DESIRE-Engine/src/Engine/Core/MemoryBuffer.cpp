#include "Engine/stdafx.h"
#include "Engine/Core/MemoryBuffer.h"

MemoryBuffer::MemoryBuffer(void *data, size_t size)
	: data(static_cast<char*>(data))
	, size(size)
{

}

MemoryBuffer::MemoryBuffer(size_t size)
	: data(static_cast<char*>(malloc(size)))
	, size(size)
{

}

MemoryBuffer::MemoryBuffer(MemoryBuffer&& other)
{
	data = other.data;
	size = other.size;
	other.data = nullptr;
	other.size = 0;
}

MemoryBuffer::~MemoryBuffer()
{
	free(data);
}

MemoryBuffer& MemoryBuffer::operator =(MemoryBuffer&& other)
{
	free(data);

	data = other.data;
	size = other.size;
	other.data = nullptr;
	other.size = 0;

	return *this;
}

MemoryBuffer MemoryBuffer::CreateFromDataCopy(const void *dataToCopy, size_t size)
{
	MemoryBuffer buffer(size);
	memcpy(buffer.data, dataToCopy, size);
	return buffer;
}
