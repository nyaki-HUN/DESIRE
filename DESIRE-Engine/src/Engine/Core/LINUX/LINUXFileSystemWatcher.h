#pragma once

#include "Core/fs/FileSystemWatcher.h"

class LINUXFileSystemWatcher : public FileSystemWatcher
{
public:
	int wd;

	static int inotifyFD;		// inotify file descriptor
};
