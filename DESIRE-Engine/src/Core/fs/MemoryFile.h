#pragma once

#include "Core/fs/FilePtr_fwd.h"
#include "Core/fs/IReadFile.h"

class MemoryFile : public IReadFile
{
public:
	// Create MemoryFile from a buffer (the buffer will be deleted by MemoryFile)
	MemoryFile(void *buffer, int64_t size);
	// Create MemoryFile by loading the given amount of data from the file into memory (when size is -1 the entire file is loaded)
	MemoryFile(ReadFilePtr& file, int64_t size = -1);
	~MemoryFile();

	bool Seek(int64_t offset, ESeekOrigin origin = ESeekOrigin::CURRENT) override;

	void ReadBufferAsync(void *buffer, size_t size, std::function<void()> callback) override;
	size_t ReadBuffer(void *buffer, size_t size) override;

private:
	char *data;
};
