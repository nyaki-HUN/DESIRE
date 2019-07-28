#pragma once

#include "Engine/Core/FS/FilePtr_fwd.h"
#include "Engine/Core/FS/IReadFile.h"

#include <memory>

class MemoryFile : public IReadFile
{
public:
	// Create MemoryFile from a buffer (the buffer will be deleted by MemoryFile)
	MemoryFile(std::unique_ptr<uint8_t[]> data, int64_t dataSize);
	// Create MemoryFile by loading the given amount of data from the file into memory (when size is -1 the entire file is loaded)
	MemoryFile(ReadFilePtr& file, int64_t size = -1);

	bool Seek(int64_t offset, ESeekOrigin origin = ESeekOrigin::Current) override;

	void ReadBufferAsync(void* buffer, size_t size, std::function<void()> callback) override;
	size_t ReadBuffer(void* buffer, size_t size) override;

private:
	std::unique_ptr<uint8_t[]> data = nullptr;
};
