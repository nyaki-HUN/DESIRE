#include "stdafx.h"
#include "Core/LINUX/LINUXFile.h"
#include "Core/fs/FileSystem.h"

#define _LARGEFILE64_SOURCE
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

LINUXFile::LINUXFile(int fileDesc, int64_t fileSize, const String& filename)
	: IReadFile(fileSize, filename)
	, fileDesc(fileDesc)
{
	ASSERT(fileDesc != -1);
}

LINUXFile::~LINUXFile()
{
	close(fileDesc);
}

bool LINUXFile::Seek(int64_t offset, ESeekOrigin origin)
{
	const int mapping[] =
	{
		SEEK_SET,
		SEEK_CUR,
		SEEK_END
	};
	ASSERT((size_t)origin < DESIRE_ASIZEOF(mapping));
	const int whence = mapping[(int)origin];

	off64_t result = lseek64(fileDesc, offset, whence);
	if(result == -1)
	{
		return false;
	}

	position = result;
	return true;
}

void LINUXFile::ReadBufferAsync(void *buffer, size_t size, std::function<void()> callback)
{
	ASSERT(callback != nullptr);
	ReadBuffer(buffer, size);
	callback();
}

size_t LINUXFile::ReadBuffer(void *buffer, size_t size)
{
	ASSERT(buffer != nullptr);
	ssize_t numBytesRead = read(fileDesc, buffer, size);
	if(numBytesRead < 0)
	{
		LOG_ERROR("Failed to read from file (Error: %d)", errno);
		return 0;
	}

	position += numBytesRead;
	return numBytesRead;
}

size_t LINUXFile::WriteBuffer(const void *buffer, size_t size)
{
	ASSERT(buffer != nullptr);
	ssize_t numBytesWritten = write(fileDesc, buffer, size);
	if(numBytesWritten < 0)
	{
		LOG_ERROR("Failed to write to file (Error: %d)", errno);
		return 0;
	}

	position += numBytesWritten;
	return numBytesWritten;
}

// --------------------------------------------------------------------------------------------------------------------
//	FileSystem
// --------------------------------------------------------------------------------------------------------------------

ReadFilePtr FileSystem::OpenNative(const String& filename)
{
	int fileDesc = open64(filename.c_str(), O_RDONLY);
	if(fileDesc == -1)
	{
		LOG_ERROR("Failed to open file %s (Error: %d)", filename.c_str(), errno);
		return nullptr;
	}

	stat64 fileStat;
	if(fstat64(fileDesc, &fileStat) == -1)
	{
		LOG_ERROR("Failed to get file size of %s (Error: %d)", filename.c_str(), errno);
		close(fileDesc);
		return nullptr;
	}

	return std::make_unique<LINUXFile>(fileDesc, fileStat.st_size, filename);
}

WriteFilePtr FileSystem::CreateWriteFile(const String& filename)
{
	// 0644 - RW for owner, R for group and others
	int fileDesc = creat64(filename.c_str(), 0644);
	if(fileDesc == -1)
	{
		LOG_ERROR("Failed to open file %s (Error: %d)", filename.c_str(), errno);
		return nullptr;
	}

	return std::make_unique<LINUXFile>(fileDesc, 0, filename);
}

void FileSystem::SetupDirectories()
{
	char exePath[DESIRE_MAX_PATH_LEN] = {};

	// Find the real executable by reading the process symlink
	char str[DESIRE_MAX_PATH_LEN] = { 0 };
	snprintf(str, sizeof(str), "/proc/%d/exe", getpid());
	const int len = readlink(str, exePath, DESIRE_MAX_PATH_LEN - 1);
	if(len > 0)
	{
		// readlink() doesn't null-terminate
		exePath[len] = '\0';

		char *slash = std::strrchr(exePath, '/');
		if(slash != nullptr)
		{
			slash++;
			*slash = '\0';
		}
	}
	else
	{
		exePath[0] = '\0';
	}

	appDir = exePath;
}
