#pragma once

#include "Engine/Core/FS/IFileSource.h"
#include "Engine/Core/String/DynamicString.h"

#include <map>

class FileSourceZip : public IFileSource
{
public:
	FileSourceZip(ReadFilePtr spZipFile, int32_t flags);
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

	std::map<DynamicString, ZipFileEntry> m_fileList;
	ReadFilePtr m_spZipFile;
	int32_t m_flags;
};
