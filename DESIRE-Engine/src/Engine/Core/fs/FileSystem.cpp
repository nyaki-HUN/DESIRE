#include "Engine/stdafx.h"
#include "Engine/Core/fs/FileSystem.h"
#include "Engine/Core/fs/IReadFile.h"
#include "Engine/Core/fs/FileSourceZip.h"

FileSystem::FileSystem()
{
	SetupDirectories();
}

FileSystem::~FileSystem()
{

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

	String filenameWithPath;
	switch(location)
	{
		case ELocation::APP_DIR:	filenameWithPath = appDir + filename; break;
		case ELocation::DATA_DIR:	filenameWithPath = dataDir + filename; break;
		case ELocation::CACHE_DIR:	filenameWithPath = cacheDir + filename; break;
	}

	return OpenNative(filenameWithPath);
}

void FileSystem::AddFileSource(IFileSource *fileSource)
{
	fileSources.push_back(fileSource);
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

const String& FileSystem::GetAppDirectory()
{
	return appDir;
}
