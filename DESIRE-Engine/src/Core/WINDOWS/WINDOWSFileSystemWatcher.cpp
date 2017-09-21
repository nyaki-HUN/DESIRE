#include "stdafx.h"
#include "Core/WINDOWS/WINDOWSFileSystemWatcher.h"

void WINDOWSFileSystemWatcher::RefreshWatch()
{
	BOOL succeeded = ReadDirectoryChangesW(
		dirHandle,
		buffer,
		sizeof(buffer),
		TRUE,
		FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_SIZE | FILE_NOTIFY_CHANGE_CREATION,
		nullptr,
		&overlapped,
		&WINDOWSFileSystemWatcher::CompletionCallback);

	isActive = (succeeded == TRUE);
}

void CALLBACK WINDOWSFileSystemWatcher::CompletionCallback(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped)
{
	if(dwNumberOfBytesTransfered == 0)
	{
		return;
	}

	WINDOWSFileSystemWatcher *thisPtr = static_cast<WINDOWSFileSystemWatcher*>(lpOverlapped->hEvent);

	if(dwErrorCode == ERROR_SUCCESS)
	{
		char filename[MAX_PATH];
		FILE_NOTIFY_INFORMATION *notify = nullptr;
		size_t offset = 0;
		do
		{
			notify = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(thisPtr->buffer + offset);

			// Convert filename to UTF-8
			const int count = WideCharToMultiByte(CP_UTF8, 0, notify->FileName, notify->FileNameLength / sizeof(WCHAR), filename, MAX_PATH - 1, nullptr, nullptr);
			filename[count] = '\0';

			switch(notify->Action)
			{
				case FILE_ACTION_ADDED:
				case FILE_ACTION_RENAMED_NEW_NAME:
					thisPtr->actionCallback(FileSystemWatcher::EAction::ADDED, filename);
					break;

				case FILE_ACTION_REMOVED:
				case FILE_ACTION_RENAMED_OLD_NAME:
					thisPtr->actionCallback(FileSystemWatcher::EAction::DELETED, filename);
					break;

				case FILE_ACTION_MODIFIED:
					thisPtr->actionCallback(FileSystemWatcher::EAction::MODIFIED, filename);
					break;
			}

			offset += notify->NextEntryOffset;
		} while(notify->NextEntryOffset != 0);
	}

	if(thisPtr->isActive)
	{
		thisPtr->RefreshWatch();
	}
}

// --------------------------------------------------------------------------------------------------------------------
//	FileSystemWatcher
// --------------------------------------------------------------------------------------------------------------------

std::unique_ptr<FileSystemWatcher> FileSystemWatcher::Create(const String& directory, std::function<void(FileSystemWatcher::EAction action, const char *filename)> actionCallback)
{
	HANDLE dirHandle = CreateFileA(
		directory.c_str(),
		FILE_LIST_DIRECTORY,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		nullptr,
		OPEN_EXISTING,
		FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
		nullptr);

	if(dirHandle == INVALID_HANDLE_VALUE)
	{
		LOG_ERROR("FileSystemWatcher error: directory not found (%s)", directory.c_str());
		return std::unique_ptr<FileSystemWatcher>(nullptr);
	}

	WINDOWSFileSystemWatcher *watcher = new WINDOWSFileSystemWatcher();
	watcher->dirName = directory;
	watcher->actionCallback = actionCallback;
	watcher->dirHandle = dirHandle;
	watcher->overlapped.hEvent = watcher;			// The hEvent member of the OVERLAPPED structure is not used by the system, so we can use it
	watcher->isActive = false;

	watcher->RefreshWatch();

	return std::unique_ptr<FileSystemWatcher>(watcher);
}

void FileSystemWatcher::Update()
{
	MsgWaitForMultipleObjectsEx(0, NULL, 0, QS_ALLINPUT, MWMO_ALERTABLE);
}

void FileSystemWatcher::Destroy()
{
	WINDOWSFileSystemWatcher *watcher = static_cast<WINDOWSFileSystemWatcher*>(this);

	if(watcher->isActive)
	{
		watcher->isActive = false;
		CancelIo(watcher->dirHandle);
		if(!HasOverlappedIoCompleted(&watcher->overlapped))
		{
			SleepEx(5, TRUE);
		}
	}

	CloseHandle(watcher->dirHandle);
}
