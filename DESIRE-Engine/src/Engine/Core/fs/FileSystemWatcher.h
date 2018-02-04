#pragma once

#include "Engine/Core/String.h"

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
		OnDestroy();
	}

	static std::unique_ptr<FileSystemWatcher> Create(const String& directory, std::function<void(FileSystemWatcher::EAction action, const String& filename)> actionCallback);

	static void Update();

protected:
	void OnDestroy();

	String dirName;
	std::function<void(FileSystemWatcher::EAction action, const String& filename)> actionCallback;
};
