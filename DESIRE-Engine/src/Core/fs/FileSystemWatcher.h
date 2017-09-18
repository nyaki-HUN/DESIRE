#pragma once

#include "Core/String.h"

#include <functional>

class FileSystemWatcher
{
protected:
	FileSystemWatcher() {}

public:
	enum class EAction
	{
		ADDED,
		DELETED,
		MODIFIED
	};

	~FileSystemWatcher()
	{
		Destroy();
	}

	static std::unique_ptr<FileSystemWatcher> Create(const String& directory, std::function<void(FileSystemWatcher::EAction action, const char *filename)> actionCallback);

	void Update();

protected:
	void Destroy();

	String dirName;
	std::function<void(FileSystemWatcher::EAction action, const char *filename)> actionCallback;
};
