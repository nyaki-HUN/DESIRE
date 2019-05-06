#include "Engine/stdafx.h"

#if defined(DESIRE_PLATFORM_WINDOWS)

#include "Engine/Core/fs/FileSystemWatcher.h"
#include "Engine/Core/WINDOWS/os.h"
#include "Engine/Core/String/StackString.h"

class FileSystemWatcherImpl
{
public:
	void RefreshWatch();
	static void CALLBACK CompletionCallback(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped);

	HANDLE dirHandle;
	OVERLAPPED overlapped;

	uint8_t buffer[8 * 1024];
	bool isActive;
};

void FileSystemWatcherImpl::RefreshWatch()
{
	BOOL succeeded = ReadDirectoryChangesW(
		dirHandle,
		buffer,
		sizeof(buffer),
		TRUE,
		FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_SIZE | FILE_NOTIFY_CHANGE_CREATION,
		nullptr,
		&overlapped,
		&FileSystemWatcherImpl::CompletionCallback);

	isActive = (succeeded == TRUE);
}

void CALLBACK FileSystemWatcherImpl::CompletionCallback(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped)
{
	if(dwNumberOfBytesTransfered == 0)
	{
		return;
	}

	FileSystemWatcher *watcher = static_cast<FileSystemWatcher*>(lpOverlapped->hEvent);

	if(dwErrorCode == ERROR_SUCCESS)
	{
		char str[DESIRE_MAX_PATH_LEN];
		FILE_NOTIFY_INFORMATION *notify = nullptr;
		size_t offset = 0;
		do
		{
			notify = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(watcher->impl->buffer + offset);

			// Convert filename to UTF-8
			const int count = WideCharToMultiByte(CP_UTF8, 0, notify->FileName, notify->FileNameLength / sizeof(WCHAR), str, DESIRE_MAX_PATH_LEN - 1, nullptr, nullptr);
			StackString<DESIRE_MAX_PATH_LEN> filename(str, count);
			filename.ReplaceAllChar('\\', '/');

			switch(notify->Action)
			{
				case FILE_ACTION_ADDED:
				case FILE_ACTION_RENAMED_NEW_NAME:
					watcher->actionCallback(FileSystemWatcher::EAction::Added, filename);
					break;

				case FILE_ACTION_REMOVED:
				case FILE_ACTION_RENAMED_OLD_NAME:
					watcher->actionCallback(FileSystemWatcher::EAction::Deleted, filename);
					break;

				case FILE_ACTION_MODIFIED:
					watcher->actionCallback(FileSystemWatcher::EAction::Modified, filename);
					break;
			}

			offset += notify->NextEntryOffset;
		} while(notify->NextEntryOffset != 0);
	}

	if(watcher->impl->isActive)
	{
		watcher->impl->RefreshWatch();
	}
}

FileSystemWatcher::FileSystemWatcher()
	: impl(std::make_unique<FileSystemWatcherImpl>())
{
	
}

FileSystemWatcher::~FileSystemWatcher()
{
	if(impl->isActive)
	{
		impl->isActive = false;
		CancelIo(impl->dirHandle);
		if(!HasOverlappedIoCompleted(&impl->overlapped))
		{
			SleepEx(5, TRUE);
		}
	}

	CloseHandle(impl->dirHandle);
}

std::unique_ptr<FileSystemWatcher> FileSystemWatcher::Create(const String& directory, std::function<void(FileSystemWatcher::EAction action, const String& filename)> actionCallback)
{
	HANDLE dirHandle = CreateFileA(
		directory.Str(),
		FILE_LIST_DIRECTORY,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		nullptr,
		OPEN_EXISTING,
		FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
		nullptr);

	if(dirHandle == INVALID_HANDLE_VALUE)
	{
		LOG_ERROR("FileSystemWatcher error: directory not found (%s)", directory.Str());
		return std::unique_ptr<FileSystemWatcher>(nullptr);
	}

	FileSystemWatcher *watcher = new FileSystemWatcher();
	watcher->actionCallback = actionCallback;
	watcher->impl->dirHandle = dirHandle;
	watcher->impl->overlapped.hEvent = watcher;			// The hEvent member of the OVERLAPPED structure is not used by the system, so we can use it
	watcher->impl->isActive = false;

	watcher->impl->RefreshWatch();

	return std::unique_ptr<FileSystemWatcher>(watcher);
}

void FileSystemWatcher::UpdateAll()
{
	MsgWaitForMultipleObjectsEx(0, NULL, 0, QS_ALLINPUT, MWMO_ALERTABLE);
}

#endif	// #if defined(DESIRE_PLATFORM_WINDOWS)