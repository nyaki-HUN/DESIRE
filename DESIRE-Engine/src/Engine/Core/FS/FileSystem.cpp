#include "Engine/stdafx.h"
#include "Engine/Core/FS/FileSystem.h"
#include "Engine/Core/FS/IFileSource.h"
#include "Engine/Core/FS/IReadFile.h"
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
	static FileSystem s_instance;
	return &s_instance;
}

ReadFilePtr FileSystem::Open(const String& filename, ELocation location)
{
	for(std::unique_ptr<IFileSource>& fileSource : fileSources)
	{
		ReadFilePtr file = fileSource->OpenFile(filename);
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

void FileSystem::AddFileSource(std::unique_ptr<IFileSource> fileSource)
{
	fileSources.Add(std::move(fileSource));
}

const String& FileSystem::GetAppDirectory() const
{
	return appDir;
}
