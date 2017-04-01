#include "stdafx.h"
#include "Core/WINDOWS/WINDOWSFile.h"
#include "Core/fs/FileSystem.h"

WINDOWSFile::WINDOWSFile(HANDLE hFile, int64_t fileSize)
	: IReadFile(fileSize)
	, hFile(hFile)
{
	ASSERT(hFile != INVALID_HANDLE_VALUE);
}

WINDOWSFile::~WINDOWSFile()
{
	CloseHandle(hFile);
}

bool WINDOWSFile::Seek(int64_t offset, ESeekOrigin origin)
{
	const DWORD mapping[] =
	{
		FILE_BEGIN,
		FILE_CURRENT,
		FILE_END
	};
	ASSERT((size_t)origin < DESIRE_ASIZEOF(mapping));
	const DWORD moveMethod = mapping[(int)origin];

	LARGE_INTEGER fileOffset;
	fileOffset.QuadPart = offset;
	BOOL succeeded = SetFilePointerEx(hFile, fileOffset, &fileOffset, moveMethod);
	if(!succeeded)
	{
		LOG_ERROR_WITH_WIN32_ERRORCODE("Failed to seek file");
		return false;
	}

	position = fileOffset.QuadPart;
	return true;
}

void WINDOWSFile::ReadBufferAsync(void *buffer, size_t size, std::function<void()> callback)
{
	ASSERT(callback != nullptr);
	ReadBuffer(buffer, size);
	callback();
}

size_t WINDOWSFile::ReadBuffer(void *buffer, size_t size)
{
	ASSERT(buffer != nullptr);
	DWORD numBytesRead = 0;
	BOOL result = ReadFile(hFile, buffer, (DWORD)size, &numBytesRead, nullptr);
	if(result == FALSE)
	{
		LOG_ERROR_WITH_WIN32_ERRORCODE("Failed to read from file");
		return 0;
	}

	position += numBytesRead;
	return numBytesRead;
}

size_t WINDOWSFile::WriteBuffer(const void *buffer, size_t size)
{
	ASSERT(buffer != nullptr);
	DWORD numBytesWritten = 0;
	BOOL result = WriteFile(hFile, buffer, (DWORD)size, &numBytesWritten, nullptr);
	if(result == FALSE)
	{
		LOG_ERROR_WITH_WIN32_ERRORCODE("Failed to write to file");
		return 0;
	}

	position += numBytesWritten;
	return numBytesWritten;
}

ReadFilePtr FileSystem::OpenNative(const char *filename, ELocation location)
{
	ASSERT(location == ELocation::APP_DIR);

	HANDLE hFile = CreateFileA(filename, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		LOG_ERROR_WITH_WIN32_ERRORCODE("Failed to open file %s", filename);
		return nullptr;
	}

	FILE_STANDARD_INFO finfo = {};
	if(GetFileInformationByHandleEx(hFile, FileStandardInfo, &finfo, sizeof(finfo)) == 0)
	{
		LOG_ERROR_WITH_WIN32_ERRORCODE("Failed to get file size of %s", filename);
		CloseHandle(hFile);
		return nullptr;
	}

	return std::make_unique<WINDOWSFile>(hFile, finfo.EndOfFile.QuadPart);
}

WriteFilePtr FileSystem::CreateWriteFile(const char *filename, ELocation location)
{
	ASSERT(location == ELocation::APP_DIR);

	HANDLE hFile = CreateFileA(filename, GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		LOG_ERROR_WITH_WIN32_ERRORCODE("Failed to open file %s", filename);
		return nullptr;
	}

	return std::make_unique<WINDOWSFile>(hFile, 0);
}
