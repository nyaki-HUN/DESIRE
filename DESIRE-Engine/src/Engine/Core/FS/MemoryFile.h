#pragma once

#include "Engine/Core/FS/FilePtr_fwd.h"
#include "Engine/Core/FS/IReadFile.h"

class MemoryFile : public IReadFile
{
public:
	// Create MemoryFile from a buffer
	MemoryFile(std::unique_ptr<uint8_t[]> spData, size_t dataSize);
	// Create MemoryFile by loading the given amount of data from the file into memory (or the entire file if sizeToRead is not specified)
	MemoryFile(ReadFilePtr& spFile, size_t sizeToRead = SIZE_MAX);

	bool Seek(int64_t offset, ESeekOrigin origin = ESeekOrigin::Current) override;

	void ReadBufferAsync(void* pBuffer, size_t size, std::function<void()> callback) override;
	size_t ReadBuffer(void* pBuffer, size_t size) override;

private:
	std::unique_ptr<uint8_t[]> m_spData = nullptr;
};
