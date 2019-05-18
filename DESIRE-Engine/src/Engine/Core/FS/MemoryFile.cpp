#include "Engine/stdafx.h"
#include "Engine/Core/FS/MemoryFile.h"

MemoryFile::MemoryFile(void *buffer, int64_t size)
	: IReadFile(size)
	, data(static_cast<char*>(buffer))
{
	ASSERT(size >= 0);
	ASSERT(data != nullptr);
}

MemoryFile::MemoryFile(ReadFilePtr& file, int64_t size)
	: IReadFile(0)
	, data(nullptr)
{
	if(size < 0)
	{
		size = file->GetSize();
	}

	ASSERT(size >= 0 && size < SIZE_MAX);
	const size_t sizeToRead = static_cast<size_t>(size);
	data = (char*)malloc(sizeToRead);
	size_t numBytesRead = file->ReadBuffer(data, sizeToRead);
	ASSERT(numBytesRead == sizeToRead);
	fileSize = static_cast<int64_t>(numBytesRead);
}

MemoryFile::~MemoryFile()
{
	free(data);
}

bool MemoryFile::Seek(int64_t offset, ESeekOrigin origin)
{
	int64_t newPos = -1;
	switch(origin)
	{
		case ESeekOrigin::Begin:	newPos = offset; break;
		case ESeekOrigin::Current:	newPos = position + offset; break;
		case ESeekOrigin::End:		newPos = fileSize + offset; break;
	}

	if(newPos < 0 || newPos > fileSize)
	{
		return false;
	}

	position = newPos;
	return true;
}

void MemoryFile::ReadBufferAsync(void *buffer, size_t size, std::function<void()> callback)
{
	// No need for real async read
	ASSERT(callback != nullptr);
	ReadBuffer(buffer, size);
	callback();
}

size_t MemoryFile::ReadBuffer(void *buffer, size_t size)
{
	const size_t remainingSize = static_cast<size_t>(fileSize - position);
	size = std::min(size, remainingSize);

	memcpy(buffer, data + position, size);
	position += size;
	return size;
}
