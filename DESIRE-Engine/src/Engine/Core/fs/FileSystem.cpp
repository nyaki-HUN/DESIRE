#include "Engine/stdafx.h"
#include "Engine/Core/fs/FileSystem.h"
#include "Engine/Core/fs/IReadFile.h"
#include "Engine/Core/fs/FileSourceZip.h"
#include "Engine/Core/String/StackString.h"

FileSystem::FileSystem()
{
	SetupDirectories();
}

FileSystem::~FileSystem()
{

}

FileSystem* FileSystem::Get()
{
	//	Singleton behaviour using lazy initialization. In C++11 standard, this is thread safe. 
	//	§6.7[stmt.dcl] p4: "If control enters the declaration concurrently while the variable is being initialized, the concurrent execution shall wait for completion of the initialization."
	static FileSystem instance;
	return &instance;
}

ReadFilePtr FileSystem::Open(const String& filename, ELocation location)
{
	for(IFileSource *fileSrc : fileSources)
	{
		ReadFilePtr file = fileSrc->OpenFile(filename);
		if(file != nullptr)
		{
			return file;
		}
	}

	StackString<DESIRE_MAX_PATH_LEN> filenameWithPath;
	switch(location)
	{
		case ELocation::AppDir:		filenameWithPath = appDir; break;
		case ELocation::DataDir:	filenameWithPath = dataDir; break;
		case ELocation::CacheDir:	filenameWithPath = cacheDir; break;
	}
	filenameWithPath += filename;

	return OpenNative(filenameWithPath);
}

void FileSystem::AddFileSource(IFileSource *fileSource)
{
	fileSources.Add(fileSource);
}

bool FileSystem::AddDirectoryFileSource(const String& dir, int fileSourceFlags)
{
	DESIRE_TODO("implement");
	return false;
}

bool FileSystem::AddZipFileSource(const String& zipFilename, int fileSourceFlags)
{
	ReadFilePtr file = Open(zipFilename);
	if(file != nullptr)
	{
		FileSourceZip *fileSource = new FileSourceZip(std::move(file), fileSourceFlags);
		bool success = fileSource->Load();
		if(success)
		{
			AddFileSource(fileSource);
			return true;
		}
		delete fileSource;
	}

	return false;
}

const String& FileSystem::GetAppDirectory() const
{
	return appDir;
}
