#pragma once

#include "Engine/Core/fs/IFileSource.h"
#include "Engine/Core/DynamicString.h"

#include <map>

class WritableString;

class FileSourceZip : public IFileSource
{
public:
	FileSourceZip(ReadFilePtr zipFile, int flags);
	~FileSourceZip() override;

	bool Load();

	ReadFilePtr OpenFile(const String& filename) override;

private:
	struct ZipFileEntry
	{
		int64_t offsetInFile;		// Position of compressed data in the file
		uint32_t compressedSize;
		uint32_t uncompressedSize;
		int16_t compressionMethod;
	};

	void ProcessLocalHeaders();
	void ConvertFilename(WritableString& filename);

	std::map<DynamicString, ZipFileEntry> fileList;
	ReadFilePtr zipFile;
	int flags;
};
