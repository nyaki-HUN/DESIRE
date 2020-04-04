#include "Engine/stdafx.h"
#include "Engine/Compression/FileSourceZip.h"

#include "Engine/Compression/CompressionManager.h"
#include "Engine/Core/FS/FileSystem.h"		// for EFileSourceFlags
#include "Engine/Core/FS/MemoryFile.h"
#include "Engine/Core/String/StackString.h"

constexpr int kZipSignatureCentralDirectoryFileHeader	= 0x02014b50;	//'PK12'
constexpr int kZipSignatureLocalFileHeader				= 0x04034b50;	//'PK34'
constexpr int kZipSignatureEndOfCentralDirectory		= 0x06054b50;	//'PK56'

#if DESIRE_PLATFORM_WINDOWS
	#include <PshPack1.h>
#endif	// #if DESIRE_PLATFORM_WINDOWS

/*
	The structure of a PKZip file is taken from:
	https://users.cs.jmu.edu/buchhofp/forensics/formats/pkzip.html

	fileModificationTime - stored in standard MS-DOS format:
		Bits 00-04: seconds divided by 2
		Bits 05-10: minute
		Bits 11-15: hour
	fileModificationDate - stored in standard MS-DOS format:
		Bits 00-04: day
		Bits 05-08: month
		Bits 09-15: years from 1980
*/

struct ZipDataDescriptor
{
	int crc32;
	uint32_t compressedSize;
	uint32_t uncompressedSize;
} DESIRE_ATTRIBUTE_PACKED;

struct ZipLocalFileHeader
{
	enum EFlag
	{
		FLAG_ENCRYPTED				= 1 << 0,
		FLAG_COMPRESSION_OPTION		= 1 << 1,
		FLAG_COMPRESSION_OPTION2	= 1 << 2,
		FLAG_DATA_DESCRIPTOR		= 1 << 3,
		FLAG_ENHANCED_DEFLATION		= 1 << 4,
		FLAG_COMPRESSED_PATCH_DATA	= 1 << 5,
		FLAG_STRONG_ENCRYPTION		= 1 << 6,
		FLAG_LANGUAGE_ENCODING		= 1 << 11,
		FLAG_MASK_HEADER_VALUES		= 1 << 13,
	};

	int32_t signature;
	int16_t versionNeededToExtract;
	int16_t flags;
	int16_t compressionMethod;
	uint16_t fileModificationTime;
	uint16_t fileModificationDate;
	ZipDataDescriptor dataDescriptor;
	uint16_t filenameLength;
	uint16_t extraFieldLength;
} DESIRE_ATTRIBUTE_PACKED;

struct ZipCentralDirectoryFileHeader
{
	int32_t signature;
	int16_t versionMadeBy;
	int16_t versionNeededToExtract;
	int16_t flags;
	int16_t compressionMethod;
	uint16_t fileModificationTime;
	uint16_t fileModificationDate;
	ZipDataDescriptor dataDescriptor;
	uint16_t filenameLength;
	uint16_t extraFieldLength;
	uint16_t commentLength;
	int16_t diskStart;
	int16_t internalAttr;
	int32_t externalAttr;
	int32_t offsetOfLocalHeader;
} DESIRE_ATTRIBUTE_PACKED;

struct ZipEndOfCentralDirectoryRecord
{
	int32_t signature;
	int16_t diskNumber;
	int16_t diskStart;
	int16_t numCentralDirectoriesOnThisDisk;
	int16_t numCentralDirectories;
	int32_t centralDirectorySize;
	int32_t offsetOfcentralDirectoryStart;
	uint16_t zipCommentLength;
} DESIRE_ATTRIBUTE_PACKED;

#if DESIRE_PLATFORM_WINDOWS
	#include <PopPack.h>
#endif	// #if DESIRE_PLATFORM_WINDOWS

FileSourceZip::FileSourceZip(ReadFilePtr zipFile, int flags)
	: zipFile(std::move(zipFile))
	, flags(flags)
{

}

FileSourceZip::~FileSourceZip()
{

}

bool FileSourceZip::Load()
{
	if(zipFile == nullptr)
	{
		return false;
	}

	// Load central directory record
	zipFile->Seek(-(int64_t)sizeof(ZipEndOfCentralDirectoryRecord), IReadFile::ESeekOrigin::End);
	ZipEndOfCentralDirectoryRecord record;
	zipFile->ReadBuffer(&record, sizeof(ZipEndOfCentralDirectoryRecord));
	if(record.signature != kZipSignatureEndOfCentralDirectory)
	{
		LOG_ERROR("The zip file contains comment which is not supported");
		return false;
	}

	zipFile->Seek(record.offsetOfcentralDirectoryStart, IReadFile::ESeekOrigin::Begin);

	for(int16_t i = 0; i < record.numCentralDirectories; ++i)
	{
		// Process the Central Directory Header
		ZipCentralDirectoryFileHeader centralDirHeader;
		zipFile->ReadBuffer(&centralDirHeader, sizeof(ZipCentralDirectoryFileHeader));
		ASSERT(centralDirHeader.signature == kZipSignatureCentralDirectoryFileHeader);
		if(centralDirHeader.signature == kZipSignatureCentralDirectoryFileHeader)
		{
			const int64_t currPos = zipFile->Tell();
			zipFile->Seek(centralDirHeader.offsetOfLocalHeader, IReadFile::ESeekOrigin::Begin);
			ProcessLocalHeaders();
			zipFile->Seek(currPos + centralDirHeader.filenameLength + centralDirHeader.extraFieldLength + centralDirHeader.commentLength, IReadFile::ESeekOrigin::Begin);
		}
	}

	return true;
}

ReadFilePtr FileSourceZip::OpenFile(const String& filename)
{
	StackString<DESIRE_MAX_PATH_LEN> filenameToFind = filename;
	ConvertFilename(filenameToFind);

	const auto it = fileList.find(filenameToFind);
	if(it == fileList.end())
	{
		return nullptr;
	}

	const ZipFileEntry& entry = it->second;

	// 00 : No compression
	// 01 : Shrunk
	// 02 : Reduced with compression factor 1
	// 03 : Reduced with compression factor 2
	// 04 : Reduced with compression factor 3
	// 05 : Reduced with compression factor 4
	// 06 : Imploded
	// 07 : Reserved for Tokenizing compression algorithm
	// 08 : Deflated
	// 09 : Enhanced Deflated using Deflate64(tm)
	// 10 : PKWare DCL (Data Compression Library) Imploded
	// 11 : Reserved
	// 12 : Compressed using BZIP2
	// 13 : Reserved
	// 14 : LZMA
	// 15-17 : Reserved
	// 18 : Compressed using IBM TERSE
	// 19 : IBM LZ77 z
	// 95 : xz compression
	// 96 : Jpeg compression
	// 97 : WavPack
	// 98 : PPMd version I, Rev 1

	switch(entry.compressionMethod)
	{
		case 0:		// No compression
		{
			ASSERT(entry.compressedSize == entry.uncompressedSize);

			zipFile->Seek(entry.offsetInFile, IReadFile::ESeekOrigin::Begin);
			return std::make_unique<MemoryFile>(zipFile, entry.uncompressedSize);
		}

		case 8:		// Deflated
		{
			std::unique_ptr<Compression> zlibRawDeflate = CompressionManager::CreateCompression("deflate");
			if(zlibRawDeflate != nullptr)
			{
				std::unique_ptr<uint8_t[]> compressedData = std::make_unique<uint8_t[]>(entry.compressedSize);
				std::unique_ptr<uint8_t[]> decompressedData = std::make_unique<uint8_t[]>(entry.uncompressedSize);
				if(compressedData == nullptr || decompressedData == nullptr)
				{
					LOG_ERROR("Not enough memory to decompress file: %s", it->first.Str());
					return nullptr;
				}

				zipFile->Seek(entry.offsetInFile, IReadFile::ESeekOrigin::Begin);
				zipFile->ReadBuffer(compressedData.get(), entry.compressedSize);

				const size_t decompressedSize = zlibRawDeflate->DecompressBuffer(decompressedData.get(), entry.uncompressedSize, compressedData.get(), entry.compressedSize);
				ASSERT(decompressedSize == entry.uncompressedSize);
				return std::make_unique<MemoryFile>(std::move(decompressedData), decompressedSize);
			}

			ASSERT(false && "This compression method is only supported when zlib is enabled in modules.cpp");
			break;
		}

		default:
			LOG_ERROR("File has unsupported compression method.", it->first.Str());
			break;
	}

	return nullptr;
}

void FileSourceZip::ProcessLocalHeaders()
{
	ZipLocalFileHeader header;
	zipFile->ReadBuffer(&header, sizeof(header));
	if(header.signature != kZipSignatureLocalFileHeader ||
		header.filenameLength == 0 ||
		header.filenameLength >= DESIRE_MAX_PATH_LEN)
	{
		return;
	}

	// Read filename
	StackString<DESIRE_MAX_PATH_LEN> filename;
	zipFile->ReadBuffer(filename.GetCharBufferWithSize(header.filenameLength), header.filenameLength);

	// Skip if directory
	if(filename.EndsWith('/'))
	{
		return;
	}

	// Skip extra field
	if(header.extraFieldLength != 0)
	{
		zipFile->Seek(header.extraFieldLength);
	}

	// Read data descriptor
	if(header.flags & ZipLocalFileHeader::FLAG_DATA_DESCRIPTOR)
	{
		zipFile->ReadBuffer(&header.dataDescriptor, sizeof(ZipDataDescriptor));
	}

	ZipFileEntry entry;
	entry.offsetInFile = zipFile->Tell();
	entry.compressedSize = header.dataDescriptor.compressedSize;
	entry.uncompressedSize = header.dataDescriptor.uncompressedSize;
	entry.compressionMethod = header.compressionMethod;

	ConvertFilename(filename);
	fileList.insert_or_assign(std::move(DynamicString(filename)), std::move(entry));
}

void FileSourceZip::ConvertFilename(WritableString& filename)
{
	if(flags & FileSystem::EFileSourceFlags::FILESOURCE_IGNORE_PATH)
	{
		// Remove path
		const size_t pos = filename.FindLast('/');
		if(pos != String::kInvalidPos)
		{
			filename.RemoveFrom(0, pos + 1);
		}
	}

	if(flags & FileSystem::EFileSourceFlags::FILESOURCE_IGNORE_CASE)
	{
		filename.ToLower();
	}
}
