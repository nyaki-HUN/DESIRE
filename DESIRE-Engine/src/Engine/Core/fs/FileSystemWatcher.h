#pragma once

#include <functional>
#include <memory>

class FileSystemWatcherImpl;
class String;

class FileSystemWatcher
{
public:
	enum class EAction
	{
		Added,
		Deleted,
		Modified
	};

	FileSystemWatcher(const String& directory, std::function<void(FileSystemWatcher::EAction action, const String& filename)> actionCallback);
	~FileSystemWatcher();

	static void UpdateAll();

private:
	std::function<void(FileSystemWatcher::EAction action, const String& filename)> actionCallback;
	std::unique_ptr<FileSystemWatcherImpl> impl;

	friend class FileSystemWatcherImpl;
};
