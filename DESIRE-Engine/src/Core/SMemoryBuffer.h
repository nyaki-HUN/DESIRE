#pragma once

struct SMemoryBuffer
{
	char *data;
	size_t size;

	SMemoryBuffer(void *data = nullptr, size_t size = 0)
		: data(static_cast<char*>(data))
		, size(size)
	{

	}

	SMemoryBuffer(size_t size)
		: data(static_cast<char*>(malloc(size)))
		, size(size)
	{

	}

	SMemoryBuffer(SMemoryBuffer&& other)
	{
		data = other.data;
		size = other.size;
		other.data = nullptr;
		other.size = 0;
	}

	~SMemoryBuffer()
	{
		free(data);
	}

	SMemoryBuffer& operator =(SMemoryBuffer&& other)
	{
		free(data);
	
		data = other.data;
		size = other.size;
		other.data = nullptr;
		other.size = 0;
		return *this;
	}

private:
	// Prevent copy
	SMemoryBuffer(const SMemoryBuffer& other) = delete;
	SMemoryBuffer& operator=(const SMemoryBuffer& other) = delete;
};
