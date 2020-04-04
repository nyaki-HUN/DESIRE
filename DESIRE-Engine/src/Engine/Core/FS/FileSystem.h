#pragma once

#include "Engine/Core/FS/FilePtr_fwd.h"
#include "Engine/Core/Container/Array.h"
#include "Engine/Core/MemoryBuffer.h"
#include "Engine/Core/String/DynamicString.h"

class IFileSource;

class FileSystem
{
private:
	FileSystem();
	~FileSystem();

public:
	enum EFileSourceFlags
	{
		FILESOURCE_IGNORE_PATH = 0x01,
		FILESOURCE_IGNORE_CASE = 0x02
	};

	static FileSystem* Get();

	// Opens a file for reading
	ReadFilePtr Open(const String& filename);
	ReadFilePtr OpenNative(const String& filename);

	// Creates a file for writing
	WriteFilePtr CreateWriteFileNative(const String& filename);

	MemoryBuffer LoadFileContents(const String& filename);

	void AddFileSource(std::unique_ptr<IFileSource> fileSource);

	const String& GetAppDirectory() const;

private:
	void Setup();

	Array<std::unique_ptr<IFileSource>> fileSources;

	DynamicString appDir;
};
