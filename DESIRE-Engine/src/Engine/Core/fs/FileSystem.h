#pragma once

#include "Engine/Core/fs/FilePtr_fwd.h"
#include "Engine/Core/Container/Array.h"
#include "Engine/Core/String/DynamicString.h"

class IFileSource;

class FileSystem
{
private:
	FileSystem();
	~FileSystem();

public:
	enum class ELocation
	{
		// Application directory, where the binary is located
		AppDir,

		// Folder for storing any downloaded or generated files
		// WIN32:		C:/Users/<username>/AppData/Roaming/DESIRE/<exename>
		// IOS:			/var/mobile/Applications/<XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX>/Library
		// ANDROID:		/mnt/sdcard/DESIRE/<activityname>
		DataDir,

		// Cache directory
		// WIN32:		C:/Users/<username>/AppData/Roaming/DESIRE/<exename>/Cache
		// IOS:			/var/mobile/Applications/<XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX>/Library/Caches		(this is not backed up by iTunes)
		// ANDROID:		/data/data/<packagename>/cache
		CacheDir
	};

	enum EFileSourceFlags
	{
		FILESOURCE_IGNORE_PATH = 0x01,
		FILESOURCE_IGNORE_CASE = 0x02
	};

	static FileSystem* Get();

	// Opens a file for reading
	ReadFilePtr Open(const String& filename, ELocation location = ELocation::AppDir);
	
	// Creates a file for writing
	WriteFilePtr CreateWriteFile(const String& filename);

	void AddFileSource(IFileSource *fileSource);

	// Helper functions for adding built-in file sources
	bool AddDirectoryFileSource(const String& dir, int fileSourceFlags = 0);
	bool AddZipFileSource(const String& zipFilename, int fileSourceFlags = 0);

	const String& GetAppDirectory() const;

private:
	ReadFilePtr OpenNative(const String& filename);
	void SetupDirectories();

	Array<IFileSource*> fileSources;

	DynamicString appDir;
	DynamicString dataDir;
	DynamicString cacheDir;
};
