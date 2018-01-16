#include "stdafx.h"
#include "Core/OSX/OSXFileSystemWatcher.h"

// --------------------------------------------------------------------------------------------------------------------
//	FileSystemWatcher
// --------------------------------------------------------------------------------------------------------------------

std::unique_ptr<FileSystemWatcher> FileSystemWatcher::Create(const String& directory, std::function<void(FileSystemWatcher::EAction action, const char *filename)> actionCallback)
{
	return std::unique_ptr<FileSystemWatcher>(nullptr);
}

void FileSystemWatcher::Update()
{

}

void FileSystemWatcher::OnDestroy()
{

}
