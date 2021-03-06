#include "Engine/stdafx.h"
#include "Engine/Core/FS/FileSystem.h"

#if DESIRE_PLATFORM_WINDOWS

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
		LARGE_INTEGER fileOffset = {};
		fileOffset.QuadPart = offset;

		DWORD moveMethod = FILE_CURRENT;
		switch(origin)
		{
			case ESeekOrigin::Begin:	moveMethod = FILE_BEGIN; break;
			case ESeekOrigin::Current:	moveMethod = FILE_CURRENT; break;
			case ESeekOrigin::End:		moveMethod = FILE_END; break;
		}

		BOOL succeeded = SetFilePointerEx(hFile, fileOffset, &fileOffset, moveMethod);
		if(!succeeded)
		{
			LOG_ERROR_WITH_WIN32_ERRORCODE("Failed to seek file");
			return false;
		}

		m_position = fileOffset.QuadPart;
		return true;
	}

	void ReadBufferAsync(void* buffer, size_t size, std::function<void()> callback) override
	{
		DESIRE_TODO("Implement proper async file read");
		IReadFile::ReadBufferAsync(buffer, size, callback);
	}

	size_t ReadBuffer(void* pBuffer, size_t size) override
	{
		ASSERT(pBuffer != nullptr);

		DWORD numBytesRead = 0;
		BOOL result = ReadFile(hFile, pBuffer, static_cast<DWORD>(size), &numBytesRead, nullptr);
		if(result == FALSE)
		{
			LOG_ERROR_WITH_WIN32_ERRORCODE("Failed to read from file");
			return 0;
		}

		m_position += numBytesRead;
		return numBytesRead;
	}

	size_t WriteBuffer(const void* pBuffer, size_t size) override
	{
		ASSERT(pBuffer != nullptr);

		DWORD numBytesWritten = 0;
		BOOL result = WriteFile(hFile, pBuffer, static_cast<DWORD>(size), &numBytesWritten, nullptr);
		if(result == FALSE)
		{
			LOG_ERROR_WITH_WIN32_ERRORCODE("Failed to write to file");
			return 0;
		}

		m_position += numBytesWritten;
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

	FILE_STANDARD_INFO fileInfo = {};
	if(GetFileInformationByHandleEx(hFile, FileStandardInfo, &fileInfo, sizeof(FILE_STANDARD_INFO)) == 0)
	{
		LOG_ERROR_WITH_WIN32_ERRORCODE("Failed to get file size of %s", filename.Str());
		CloseHandle(hFile);
		return nullptr;
	}

	return std::make_unique<WINDOWSFile>(hFile, fileInfo.EndOfFile.QuadPart, filename);
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
	char* pFilenameBegin = nullptr;
	GetFullPathNameA(moduleFilename, DESIRE_MAX_PATH_LEN, exeFilenameWithPath, &pFilenameBegin);
	if(pFilenameBegin)
	{
		*pFilenameBegin = '\0';
	}

	m_appDir = DynamicString(exeFilenameWithPath, strnlen_s(exeFilenameWithPath, DESIRE_MAX_PATH_LEN));
}

#endif	// #if DESIRE_PLATFORM_WINDOWS
