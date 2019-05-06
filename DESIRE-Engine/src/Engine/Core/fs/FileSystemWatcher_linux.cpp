#include "Engine/stdafx.h"

#if defined(DESIRE_PLATFORM_LINUX)

#include "Engine/Core/fs/FileSystemWatcher.h"

#include <sys/inotify.h>
#include <unistd.h>
#include <errno.h>

static int s_inotifyFD = -1;		// inotify file descriptor

class FileSystemWatcherImpl
{
public:
	int wd;
};

FileSystemWatcher::FileSystemWatcher()
	: impl(std::make_unique<FileSystemWatcherImpl>())
{

}

FileSystemWatcher::~FileSystemWatcher()
{
	inotify_rm_watch(s_inotifyFD, impl->wd);
}

std::unique_ptr<FileSystemWatcher> FileSystemWatcher::Create(const String& directory, std::function<void(FileSystemWatcher::EAction action, const String& filename)> actionCallback)
{
	if(s_inotifyFD < 0)
	{
		s_inotifyFD = inotify_init();
		if(s_inotifyFD < 0)
		{
			return std::unique_ptr<FileSystemWatcher>(nullptr);
		}
	}

	int wd = inotify_add_watch(s_inotifyFD, directory.Str(), IN_CLOSE_WRITE | IN_MOVED_FROM | IN_MOVED_TO | IN_CREATE | IN_DELETE);
	if(wd < 0)
	{
		LOG_ERROR("FileSystemWatcher error: %s", strerror(errno));
		return std::unique_ptr<FileSystemWatcher>(nullptr);
	}

	FileSystemWatcher *watcher = new FileSystemWatcher();
	watcher->actionCallback = actionCallback;
	watcher->impl->wd = wd;

	return std::unique_ptr<FileSystemWatcher>(watcher);
}

void FileSystemWatcher::UpdateAll()
{
	fd_set read;
	struct timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 0;

	FD_ZERO(&read);
	FD_SET(s_inotifyFD, &read);

	int ret = select(s_inotifyFD + 1, &read, nullptr, nullptr, &timeout);
	if(ret < 0)
	{
		LOG_ERROR("Error calling select() in FileSystemWatcher");
		return;
	}

	if(FD_ISSET(s_inotifyFD, &read))
	{
		char buffer[1024 * (sizeof(inotify_event) + FILENAME_MAX)] = {};

		const ssize_t numRead = read(s_inotifyFD, buff, BUFF_SIZE);
		ssize_t offset = 0;
		while(offset < numRead)
		{
			inotify_event *event = reinterpret_cast<inotify_event*>(buffer + offset);
			const String filename(event->name, event->len);

			FileSystemWatcher *watcher = watchers[event->wd];

			if(event->mask & IN_MOVED_TO || event->mask & IN_CREATE)
			{
				watcher->actionCallback(FileSystemWatcher::EAction::ADDED, filename);
			}
			else if(event->mask & IN_MOVED_FROM || event->mask & IN_DELETE)
			{
				watcher->actionCallback(FileSystemWatcher::EAction::DELETED, filename);
			}
			else if(event->mask & IN_CLOSE_WRITE)
			{
				watcher->actionCallback(FileSystemWatcher::EAction::MODIFIED, filename);
			}

			offset += sizeof(inotify_event) + event->len;
		}
	}
}

#endif	// #if defined(DESIRE_PLATFORM_LINUX)