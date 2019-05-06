#pragma once

#include <functional>
#include <memory>

class FileSystemWatcherImpl;
class String;

class FileSystemWatcher
{
	FileSystemWatcher();

public:
	enum class EAction
	{
		Added,
		Deleted,
		Modified
	};

	~FileSystemWatcher();

	static std::unique_ptr<FileSystemWatcher> Create(const String& directory, std::function<void(FileSystemWatcher::EAction action, const String& filename)> actionCallback);

	static void UpdateAll();

private:
	std::function<void(FileSystemWatcher::EAction action, const String& filename)> actionCallback;

	std::unique_ptr<FileSystemWatcherImpl> impl;

	friend class FileSystemWatcherImpl;
};
