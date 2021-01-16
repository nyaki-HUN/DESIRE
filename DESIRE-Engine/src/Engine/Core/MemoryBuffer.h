#pragma once

class MemoryBuffer
{
public:
	MemoryBuffer(std::unique_ptr<uint8_t[]> spData = nullptr, size_t size = 0);
	MemoryBuffer(size_t size);

	uint8_t* GetData() const;
	size_t GetSize() const;

	static MemoryBuffer CreateFromDataCopy(const void* pDataToCopy, size_t size);

private:
	std::unique_ptr<uint8_t[]> m_spData;
	size_t m_size;
};
