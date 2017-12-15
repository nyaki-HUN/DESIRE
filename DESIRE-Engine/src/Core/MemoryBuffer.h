#pragma once

#include <stdlib.h>		// for malloc and free
#include <string.h>		// for memcpy

class MemoryBuffer
{
public:
	MemoryBuffer(void *data = nullptr, size_t size = 0)
		: data(static_cast<char*>(data))
		, size(size)
	{

	}

	MemoryBuffer(size_t size)
		: data(static_cast<char*>(malloc(size)))
		, size(size)
	{

	}

	MemoryBuffer(MemoryBuffer&& other)
	{
		data = other.data;
		size = other.size;
		other.data = nullptr;
		other.size = 0;
	}

	~MemoryBuffer()
	{
		free(data);
	}

	MemoryBuffer& operator =(MemoryBuffer&& other)
	{
		free(data);
	
		data = other.data;
		size = other.size;
		other.data = nullptr;
		other.size = 0;
		return *this;
	}

	static MemoryBuffer CreateFromDataCopy(const void *dataToCopy, size_t size)
	{
		MemoryBuffer buffer(size);
		memcpy(buffer.data, dataToCopy, size);
		return buffer;
	}

	char *data;
	size_t size;

private:
	// Prevent copy
	MemoryBuffer(const MemoryBuffer& other) = delete;
	MemoryBuffer& operator=(const MemoryBuffer& other) = delete;
};
