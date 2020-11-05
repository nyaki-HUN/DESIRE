#include "Engine/stdafx.h"
#include "Engine/Core/FS/FileSystem.h"

#include "Engine/Core/FS/IFileSource.h"
#include "Engine/Core/FS/IReadFile.h"
#include "Engine/Core/String/StackString.h"

FileSystem::FileSystem()
{
	Setup();
}

FileSystem::~FileSystem()
{
}

FileSystem& FileSystem::Get()
{
	//	Singleton behaviour using lazy initialization. In C++11 standard, this is thread safe. 
	//	§6.7[stmt.dcl] p4: "If control enters the declaration concurrently while the variable is being initialized, the concurrent execution shall wait for completion of the initialization."
	static FileSystem s_instance;
	return s_instance;
}

ReadFilePtr FileSystem::Open(const String& filename)
{
	if(filename.IsEmpty())
	{
		return nullptr;
	}

	for(std::unique_ptr<IFileSource>& spFileSource : m_fileSources)
	{
		ReadFilePtr spFile = spFileSource->OpenFile(filename);
		if(spFile)
		{
			return spFile;
		}
	}

	StackString<DESIRE_MAX_PATH_LEN> filenameWithPath = m_appDir;
	filenameWithPath += filename;

	return OpenNative(filenameWithPath);
}

DynamicString FileSystem::LoadTextFile(const String& filename)
{
	ReadFilePtr spFile = Open(filename);
	if(spFile)
	{
		return spFile->ReadAllAsText();
	}

	return DynamicString();
}

MemoryBuffer FileSystem::LoadBinaryFile(const String& filename)
{
	ReadFilePtr spFile = Open(filename);
	if(spFile)
	{
		return spFile->ReadAllAsBinary();
	}

	return MemoryBuffer();
}

void FileSystem::AddFileSource(std::unique_ptr<IFileSource> spFileSource)
{
	m_fileSources.Add(std::move(spFileSource));
}

const String& FileSystem::GetAppDirectory() const
{
	return m_appDir;
}
