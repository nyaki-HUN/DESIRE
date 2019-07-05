#pragma once

class MemoryBuffer
{
public:
	MemoryBuffer(void* data = nullptr, size_t size = 0);
	MemoryBuffer(size_t size);
	MemoryBuffer(MemoryBuffer&& other);
	~MemoryBuffer();

	MemoryBuffer& operator =(MemoryBuffer&& other);

	static MemoryBuffer CreateFromDataCopy(const void* dataToCopy, size_t size);

	char* data;
	size_t size;

private:
	// Prevent copy
	MemoryBuffer(const MemoryBuffer& other) = delete;
	MemoryBuffer& operator=(const MemoryBuffer& other) = delete;
};
