#include "Engine/stdafx.h"
#include "Engine/Core/WINDOWS/WINDOWSFile.h"
#include "Engine/Core/FS/FileSystem.h"

WINDOWSFile::WINDOWSFile(HANDLE hFile, int64_t fileSize, const String& filename)
	: IReadFile(fileSize, filename)
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

void WINDOWSFile::ReadBufferAsync(void* buffer, size_t size, std::function<void()> callback)
{
	ASSERT(buffer != nullptr);
	ASSERT(callback != nullptr);

	DESIRE_TODO("Implement proper async file read");
	ReadBuffer(buffer, size);
	callback();
}

size_t WINDOWSFile::ReadBuffer(void* buffer, size_t size)
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

size_t WINDOWSFile::WriteBuffer(const void* buffer, size_t size)
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

// --------------------------------------------------------------------------------------------------------------------
//	FileSystem
// --------------------------------------------------------------------------------------------------------------------

ReadFilePtr FileSystem::OpenNative(const String& filename)
{
	HANDLE hFile = CreateFileA(filename.Str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		LOG_ERROR_WITH_WIN32_ERRORCODE("Failed to open file %s", filename.Str());
		return nullptr;
	}

	FILE_STANDARD_INFO finfo = {};
	if(GetFileInformationByHandleEx(hFile, FileStandardInfo, &finfo, sizeof(finfo)) == 0)
	{
		LOG_ERROR_WITH_WIN32_ERRORCODE("Failed to get file size of %s", filename.Str());
		CloseHandle(hFile);
		return nullptr;
	}

	return std::make_unique<WINDOWSFile>(hFile, finfo.EndOfFile.QuadPart, filename);
}

WriteFilePtr FileSystem::CreateWriteFile(const String& filename)
{
	HANDLE hFile = CreateFileA(filename.Str(), GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		LOG_ERROR_WITH_WIN32_ERRORCODE("Failed to open file %s", filename.Str());
		return nullptr;
	}

	return std::make_unique<WINDOWSFile>(hFile, 0, filename);
}

void FileSystem::SetupDirectories()
{
	char exePath[DESIRE_MAX_PATH_LEN] = {};

	const DWORD len = GetModuleFileNameA(NULL, exePath, DESIRE_MAX_PATH_LEN);
	if(len > 0 && len < DESIRE_MAX_PATH_LEN)
	{
		char* slash = std::strrchr(exePath, '\\');
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

	appDir = DynamicString(exePath, strlen(exePath));

	DESIRE_TODO("Set dataDir and cacheDir properly");
	dataDir = appDir;
	cacheDir = appDir;
}
