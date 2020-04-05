#pragma once

class MemoryBuffer
{
public:
	MemoryBuffer(std::unique_ptr<uint8_t[]> data = nullptr, size_t size = 0);
	MemoryBuffer(size_t size);

	static MemoryBuffer CreateFromDataCopy(const void* dataToCopy, size_t size);

	std::unique_ptr<uint8_t[]> ptr;
	size_t size;
};
