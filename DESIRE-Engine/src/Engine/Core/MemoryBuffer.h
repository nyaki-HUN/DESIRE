#pragma once

struct MemoryBuffer
{
	std::unique_ptr<uint8_t[]> ptr;
	size_t size;

	MemoryBuffer(std::unique_ptr<uint8_t[]> spData = nullptr, size_t size = 0);
	MemoryBuffer(size_t size);

	static MemoryBuffer CreateFromDataCopy(const void* pDataToCopy, size_t size);
};
