#include "Engine/stdafx.h"
#include "Engine/Core/FS/FileSystemWatcher.h"

#if DESIRE_PLATFORM_WINDOWS

#include "Engine/Core/String/StackString.h"
#include "Engine/Core/WINDOWS/os.h"

// --------------------------------------------------------------------------------------------------------------------
//	FileSystemWatcherImpl
// --------------------------------------------------------------------------------------------------------------------

class FileSystemWatcherImpl
{
public:
	void RefreshWatch()
	{
		BOOL succeeded = ReadDirectoryChangesExW(
			dirHandle,
			buffer,
			sizeof(buffer),
			TRUE,
			FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_CREATION,
			nullptr,
			pOverlapped,
			&FileSystemWatcherImpl::CompletionCallback,
			ReadDirectoryNotifyExtendedInformation);

		isActive = (succeeded == TRUE);
	}

	static void CALLBACK CompletionCallback(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped)
	{
		if(dwErrorCode == ERROR_OPERATION_ABORTED || lpOverlapped->hEvent == nullptr)
		{
			delete lpOverlapped;
			return;
		}

		if(dwNumberOfBytesTransfered == 0)
		{
			return;
		}

		FileSystemWatcher* pWatcher = static_cast<FileSystemWatcher*>(lpOverlapped->hEvent);

		if(dwErrorCode == ERROR_SUCCESS)
		{
			char str[DESIRE_MAX_PATH_LEN];
			FILE_NOTIFY_EXTENDED_INFORMATION* notify = nullptr;
			size_t offset = 0;
			do
			{
				notify = reinterpret_cast<FILE_NOTIFY_EXTENDED_INFORMATION*>(pWatcher->m_spImpl->buffer + offset);

				// Convert filename to UTF-8
				const int count = WideCharToMultiByte(CP_UTF8, 0, notify->FileName, notify->FileNameLength / sizeof(WCHAR), str, DESIRE_MAX_PATH_LEN - 1, nullptr, nullptr);
				StackString<DESIRE_MAX_PATH_LEN> filename(str, count);
				filename.ReplaceAllChar('\\', '/');

				switch(notify->Action)
				{
					case FILE_ACTION_ADDED:
					case FILE_ACTION_RENAMED_NEW_NAME:
						pWatcher->m_actionCallback(FileSystemWatcher::EAction::Added, filename);
						break;

					case FILE_ACTION_REMOVED:
					case FILE_ACTION_RENAMED_OLD_NAME:
						pWatcher->m_actionCallback(FileSystemWatcher::EAction::Deleted, filename);
						break;

					case FILE_ACTION_MODIFIED:
						// Don't call for empty files to filter out duplicated notifications about file writes
						if(notify->FileSize.QuadPart != 0)
						{
							pWatcher->m_actionCallback(FileSystemWatcher::EAction::Modified, filename);
						}
						break;
				}

				offset += notify->NextEntryOffset;
			} while(notify->NextEntryOffset != 0);
		}

		if(pWatcher->m_spImpl->isActive)
		{
			pWatcher->m_spImpl->RefreshWatch();
		}
	}

	HANDLE dirHandle = INVALID_HANDLE_VALUE;
	OVERLAPPED* pOverlapped = nullptr;

	uint8_t buffer[8 * 1024];
	bool isActive = false;
};

// --------------------------------------------------------------------------------------------------------------------
//	FileSystemWatcher
// --------------------------------------------------------------------------------------------------------------------

FileSystemWatcher::FileSystemWatcher(const String& directory, std::function<void(FileSystemWatcher::EAction action, const String& filename)> actionCallback)
	: m_actionCallback(actionCallback)
	, m_spImpl(std::make_unique<FileSystemWatcherImpl>())
{
	m_spImpl->pOverlapped = new OVERLAPPED();		// Will be deleted in the CompletionCallback()
	m_spImpl->pOverlapped->hEvent = this;			// The hEvent member of the OVERLAPPED structure is not used by the system, so we can use it
	m_spImpl->dirHandle = CreateFileA(
		directory.Str(),
		FILE_LIST_DIRECTORY,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		nullptr,
		OPEN_EXISTING,
		FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
		nullptr);

	if(m_spImpl->dirHandle == INVALID_HANDLE_VALUE)
	{
		LOG_ERROR("FileSystemWatcher error: directory not found (%s)", directory.Str());
		return;
	}

	m_spImpl->RefreshWatch();
}

FileSystemWatcher::~FileSystemWatcher()
{
	m_spImpl->pOverlapped->hEvent = nullptr;

	if(m_spImpl->isActive)
	{
		m_spImpl->isActive = false;
		CancelIoEx(m_spImpl->dirHandle, m_spImpl->pOverlapped);
	}
	else
	{
		delete m_spImpl->pOverlapped;
		m_spImpl->pOverlapped = nullptr;
	}

	if(m_spImpl->dirHandle != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_spImpl->dirHandle);
	}
}

void FileSystemWatcher::UpdateAll()
{
	MsgWaitForMultipleObjectsEx(0, NULL, 0, QS_ALLINPUT, MWMO_ALERTABLE);
}

#endif	// #if DESIRE_PLATFORM_WINDOWS
