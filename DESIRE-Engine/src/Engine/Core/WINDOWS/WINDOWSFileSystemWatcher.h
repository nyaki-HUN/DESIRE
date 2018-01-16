#pragma once

#include "Engine/Core/fs/FileSystemWatcher.h"
#include "Engine/Core/WINDOWS/os.h"

class WINDOWSFileSystemWatcher : public FileSystemWatcher
{
public:
	void RefreshWatch();
	static void CALLBACK CompletionCallback(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped);

	HANDLE dirHandle;
	OVERLAPPED overlapped;

	uint8_t buffer[8 * 1024];
	bool isActive;
};
