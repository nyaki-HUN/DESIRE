#include "Engine/stdafx.h"

#if DESIRE_PLATFORM_WINDOWS

#include "Engine/Core/FS/FileSystem.h"
#include "Engine/Core/FS/IReadFile.h"
#include "Engine/Core/FS/IWriteFile.h"
#include "Engine/Core/WINDOWS/os.h"

class WINDOWSFile : public IReadFile, public IWriteFile
{
public:
	WINDOWSFile(HANDLE hFile, int64_t fileSize, const String& filename)
		: IReadFile(fileSize, filename)
		, hFile(hFile)
	{
		ASSERT(hFile != INVALID_HANDLE_VALUE);
	}

	~WINDOWSFile() override
	{
		CloseHandle(hFile);
	}

	bool Seek(int64_t offset, ESeekOrigin origin) override
	{
		const DWORD mapping[] =
		{
			FILE_BEGIN,
			FILE_CURRENT,
			FILE_END
		};
		const DWORD moveMethod = mapping[static_cast<size_t>(origin)];

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

	void ReadBufferAsync(void* buffer, size_t size, std::function<void()> callback) override
	{
		DESIRE_TODO("Implement proper async file read");
		IReadFile::ReadBufferAsync(buffer, size, callback);
	}

	size_t ReadBuffer(void* buffer, size_t size) override
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

	size_t WriteBuffer(const void* buffer, size_t size) override
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

private:
	HANDLE hFile;
};

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

WriteFilePtr FileSystem::CreateWriteFileNative(const String& filename)
{
	HANDLE hFile = CreateFileA(filename.Str(), GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		LOG_ERROR_WITH_WIN32_ERRORCODE("Failed to open file %s", filename.Str());
		return nullptr;
	}

	return std::make_unique<WINDOWSFile>(hFile, 0, filename);
}

void FileSystem::Setup()
{
	char moduleFilename[DESIRE_MAX_PATH_LEN] = {};
	const DWORD len = GetModuleFileNameA(NULL, moduleFilename, DESIRE_MAX_PATH_LEN);

	char exeFilenameWithPath[DESIRE_MAX_PATH_LEN] = {};
	char* filenameBegin = nullptr;
	GetFullPathNameA(moduleFilename, DESIRE_MAX_PATH_LEN, exeFilenameWithPath, &filenameBegin);
	if(filenameBegin != nullptr)
	{
		*filenameBegin = '\0';
	}

	appDir = DynamicString(exeFilenameWithPath, strlen(exeFilenameWithPath));
}

#endif	// #if DESIRE_PLATFORM_WINDOWS
