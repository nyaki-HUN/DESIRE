#include "Engine/stdafx.h"

#if DESIRE_PLATFORM_OSX

#include "Engine/Core/FS/FileSystemWatcher.h"

// --------------------------------------------------------------------------------------------------------------------
//	FileSystemWatcherImpl
// --------------------------------------------------------------------------------------------------------------------

class FileSystemWatcherImpl
{
public:
};

// --------------------------------------------------------------------------------------------------------------------
//	FileSystemWatcher
// --------------------------------------------------------------------------------------------------------------------

FileSystemWatcher::FileSystemWatcher(const String& directory, std::function<void(FileSystemWatcher::EAction action, const String& filename)> actionCallback)
	: actionCallback(actionCallback)
	, impl(std::make_unique<FileSystemWatcherImpl>())
{

}

FileSystemWatcher::~FileSystemWatcher()
{

}

void FileSystemWatcher::UpdateAll()
{

}

#endif	// #if DESIRE_PLATFORM_OSX
