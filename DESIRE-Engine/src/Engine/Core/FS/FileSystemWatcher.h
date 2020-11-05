#pragma once

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
	std::function<void(FileSystemWatcher::EAction action, const String& filename)> m_actionCallback;
	std::unique_ptr<FileSystemWatcherImpl> m_spImpl;

	friend class FileSystemWatcherImpl;
};
