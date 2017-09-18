#include "stdafx.h"
#include "Core/LINUX/LINUXFileSystemWatcher.h"

#include <sys/inotify.h>
#include <errno.h>

int LINUXFileSystemWatcher::inotifyFD = -1;

// --------------------------------------------------------------------------------------------------------------------
//	FileSystemWatcher
// --------------------------------------------------------------------------------------------------------------------

std::unique_ptr<FileSystemWatcher> FileSystemWatcher::Create(const String& directory, std::function<void(FileSystemWatcher::EAction action, const char *filename)> actionCallback)
{
	if(inotifyFD < 0)
	{
		inotifyFD = inotify_init();
		if(inotifyFD < 0)
		{
			return std::unique_ptr<FileSystemWatcher>(nullptr);
		}
	}

	int wd = inotify_add_watch(inotifyFD, directory.c_str(), IN_CLOSE_WRITE | IN_MOVED_FROM | IN_MOVED_TO | IN_CREATE | IN_DELETE);
	if(wd < 0)
	{
		LOG_ERROR("FileSystemWatcher error: %s", strerror(errno));
		return std::unique_ptr<FileSystemWatcher>(nullptr);
	}

	LINUXFileSystemWatcher *watcher = new LINUXFileSystemWatcher();
	watcher->dirName = directory;
	watcher->actionCallback = actionCallback;
	watcher->wd = wd;

	return std::unique_ptr<FileSystemWatcher>(watcher);
}

void FileSystemWatcher::Update()
{
	LINUXFileSystemWatcher *watcher = static_cast<LINUXFileSystemWatcher*>(this);

	fd_set read;
	struct timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 0;

	FD_ZERO(&read);
	FD_SET(inotifyFD, &read);

	int ret = select(inotifyFD + 1, &read, nullptr, nullptr, &timeout);
	if(ret < 0)
	{
		LOG_ERROR("Error calling select() in FileSystemWatcher");
		return;
	}

	if(FD_ISSET(inotifyFD, &read))
	{
		char buffer[1024 * (sizeof(inotify_event) + FILENAME_MAX)] = {};

		ssize_t numRead = read(inotifyFD, buff, BUFF_SIZE);
		ssize_t offset = 0;
		while(i < len)
		{
			inotify_event *event = reinterpret_cast<inotify_event*>(buffer + offset);

			if(event->wd == watcher->wd)
			{
				// ?
			}

			if(event->mask & IN_MOVED_TO || event->mask & IN_CREATE)
			{
				thisPtr->actionCallback(FileSystemWatcher::EAction::ADDED, event->name);
			}
			else if(event->mask & IN_MOVED_FROM || event->mask & IN_DELETE)
			{
				thisPtr->actionCallback(FileSystemWatcher::EAction::DELETED, event->name);
			}
			else if(event->mask & IN_CLOSE_WRITE)
			{
				thisPtr->actionCallback(FileSystemWatcher::EAction::MODIFIED, event->name);
			}

			offset += sizeof(inotify_event) + event->len;
		}
	}
}

void FileSystemWatcher::Destroy()
{
	LINUXFileSystemWatcher *watcher = static_cast<LINUXFileSystemWatcher*>(this);

	inotify_rm_watch(inotifyFD, watcher->wd);
}
