#include "Engine/stdafx.h"

#if defined(DESIRE_PLATFORM_OSX)

#include "Engine/Core/fs/FileSystemWatcher.h"

class FileSystemWatcherImpl
{
public:
};

FileSystemWatcher::FileSystemWatcher()
	: impl(std::make_unique<FileSystemWatcherImpl>())
{

}

FileSystemWatcher::~FileSystemWatcher()
{

}

std::unique_ptr<FileSystemWatcher> FileSystemWatcher::Create(const String& directory, std::function<void(FileSystemWatcher::EAction action, const char *filename)> actionCallback)
{
	return std::unique_ptr<FileSystemWatcher>(nullptr);
}

void FileSystemWatcher::UpdateAll()
{

}

#endif	// #if defined(DESIRE_PLATFORM_OSX)
