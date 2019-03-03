#pragma once

#include <functional>

class String;

class FileSystemWatcher
{
protected:
	FileSystemWatcher() {}

public:
	enum class EAction
	{
		Added,
		Deleted,
		Modified
	};

	~FileSystemWatcher()
	{
		OnDestroy();
	}

	static std::unique_ptr<FileSystemWatcher> Create(const String& directory, std::function<void(FileSystemWatcher::EAction action, const String& filename)> actionCallback);

	static void Update();

protected:
	void OnDestroy();

	std::function<void(FileSystemWatcher::EAction action, const String& filename)> actionCallback;
};
