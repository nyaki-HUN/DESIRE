#pragma once

#include "Core/Singleton.h"
#include "Core/String.h"
#include "Core/fs/FilePtr_fwd.h"

#include <vector>

class IFileSource;

class FileSystem
{
	DESIRE_DECLARE_SINGLETON(FileSystem)

public:
	enum class ELocation
	{
		// Application directory, where the binary is located
		APP_DIR,

		// Folder for storing any downloaded or generated files
		// WIN32:		C:/Users/<username>/AppData/Roaming/DESIRE/<exename>
		// IOS:			/var/mobile/Applications/<XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX>/Library
		// ANDROID:		/mnt/sdcard/DESIRE/<activityname>
		DATA_DIR,

		// Cache directory
		// WIN32:		C:/Users/<username>/AppData/Roaming/DESIRE/<exename>/Cache
		// IOS:			/var/mobile/Applications/<XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX>/Library/Caches		(this is not backed up by iTunes)
		// ANDROID:		/data/data/<packagename>/cache
		CACHE_DIR
	};

	enum EFileSourceFlags
	{
		FILESOURCE_IGNORE_PATH = 0x01,
		FILESOURCE_IGNORE_CASE = 0x02
	};

	// Opens a file for reading
	ReadFilePtr Open(const char *filename, ELocation location = ELocation::APP_DIR);
	
	// Creates a file for writing
	WriteFilePtr CreateWriteFile(const char *filename);

	void AddFileSource(IFileSource *fileSource);

	// Helper functions for adding built-in file sources
	bool AddDirectoryFileSource(const char *dir, int fileSourceFlags = 0);
	bool AddZipFileSource(const char *zipFilename, int fileSourceFlags = 0);

	const String& GetAppDirectory();

private:
	ReadFilePtr OpenNative(const String& filename);
	void SetupDirectories();

	std::vector<IFileSource*> fileSources;

	String appDir;
	String dataDir;
	String cacheDir;
};
