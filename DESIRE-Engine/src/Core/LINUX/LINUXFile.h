#pragma once

#include "Core/fs/IReadFile.h"
#include "Core/fs/IWriteFile.h"

class LINUXFile : public IReadFile, public IWriteFile
{
public:
	LINUXFile(int fileDesc, int64_t fileSize);
	~LINUXFile();

	bool Seek(int64_t offset, ESeekOrigin origin = ESeekOrigin::CURRENT) override;

	void ReadBufferAsync(void *buffer, size_t size, std::function<void()> callback) override;
	size_t ReadBuffer(void *buffer, size_t size) override;

	size_t WriteBuffer(const void *buffer, size_t size) override;

private:
	int fileDesc;
};
