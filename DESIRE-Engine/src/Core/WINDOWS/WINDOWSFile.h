#pragma once

#include "Core/fs/IReadFile.h"
#include "Core/fs/IWriteFile.h"
#include "os.h"

class WINDOWSFile : public IReadFile, public IWriteFile
{
public:
	WINDOWSFile(HANDLE hFile, int64_t fileSize);
	~WINDOWSFile();

	// IReadFile
	bool Seek(int64_t offset, ESeekOrigin origin = ESeekOrigin::CURRENT) override;

	void ReadBufferAsync(void *buffer, size_t size, std::function<void()> callback) override;
	size_t ReadBuffer(void *buffer, size_t size) override;

	// IWriteFile
	size_t WriteBuffer(const void *buffer, size_t size) override;

private:
	HANDLE hFile;
};
