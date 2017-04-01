#include "stdafx.h"
#include "Core/fs/FileSourceZip.h"
#include "Core/fs/MemoryFile.h"
#include "Core/fs/FileSystem.h"		// for EFileSourceFlags
#include "Core/memory/IAllocator.h"

#if defined(DESIRE_ENABLE_ZLIB)
	#include "zlib/include/zlib.h"
#endif

static const int ZIP_SIGNATURE_CENTRAL_DIRECTORY_FILE_HEADER	= 0x02014b50;	//'PK12'
static const int ZIP_SIGNATURE_LOCAL_FILE_HEADER				= 0x04034b50;	//'PK34'
static const int ZIP_SIGNATURE_END_OF_CENTRAL_DIRECTORY			= 0x06054b50;	//'PK56'

#if defined(DESIRE_PLATFORM_WINDOWS)
	#include <PshPack1.h>
#endif

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

struct SZipDataDescriptor
{
	int crc32;
	uint32_t compressedSize;
	uint32_t uncompressedSize;
} DESIRE_ATTRIBUTE_PACKED;

struct SZipLocalFileHeader
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
	SZipDataDescriptor dataDescriptor;
	uint16_t filenameLength;
	uint16_t extraFieldLength;
} DESIRE_ATTRIBUTE_PACKED;

struct SZipCentralDirectoryFileHeader
{
	int32_t signature;
	int16_t versionMadeBy;
	int16_t versionNeededToExtract;
	int16_t flags;
	int16_t compressionMethod;
	uint16_t fileModificationTime;
	uint16_t fileModificationDate;
	SZipDataDescriptor dataDescriptor;
	uint16_t filenameLength;
	uint16_t extraFieldLength;
	uint16_t commentLength;
	int16_t diskStart;
	int16_t internalAttr;
	int32_t externalAttr;
	int32_t offsetOfLocalHeader;
} DESIRE_ATTRIBUTE_PACKED;

struct SZipEndOfCentralDirectoryRecord
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

#if defined(DESIRE_PLATFORM_WINDOWS)
	#include <PopPack.h>
#endif

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
	zipFile->Seek(-(int64_t)sizeof(SZipEndOfCentralDirectoryRecord), IReadFile::ESeekOrigin::END);
	SZipEndOfCentralDirectoryRecord record;
	zipFile->ReadBuffer(&record, sizeof(SZipEndOfCentralDirectoryRecord));
	if(record.signature != ZIP_SIGNATURE_END_OF_CENTRAL_DIRECTORY)
	{
		LOG_ERROR("The zip file contains comment which is not supported");
		return false;
	}

	zipFile->Seek(record.offsetOfcentralDirectoryStart, IReadFile::ESeekOrigin::BEGIN);

	for(int16_t i = 0; i < record.numCentralDirectories; ++i)
	{
		// Process the Central Directory Header
		SZipCentralDirectoryFileHeader centralDirHeader;
		zipFile->ReadBuffer(&centralDirHeader, sizeof(SZipCentralDirectoryFileHeader));
		ASSERT(centralDirHeader.signature == ZIP_SIGNATURE_CENTRAL_DIRECTORY_FILE_HEADER);
		if(centralDirHeader.signature == ZIP_SIGNATURE_CENTRAL_DIRECTORY_FILE_HEADER)
		{
			const int64_t currPos = zipFile->Tell();
			zipFile->Seek(centralDirHeader.offsetOfLocalHeader, IReadFile::ESeekOrigin::BEGIN);
			ProcessLocalHeaders();
			zipFile->Seek(currPos + centralDirHeader.filenameLength + centralDirHeader.extraFieldLength + centralDirHeader.commentLength, IReadFile::ESeekOrigin::BEGIN);
		}
	}

	return true;
}

ReadFilePtr FileSourceZip::OpenFile(const char *filename)
{
	const auto it = fileList.find(ConvertFilename(filename));
	if(it == fileList.end())
	{
		return nullptr;
	}

	const SZipFileEntry& entry = it->second;

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
	// 98 : PPMd version I, Rev 1

	switch(entry.compressionMethod)
	{
		case 0:		// No compression
		{
			ASSERT(entry.compressedSize == entry.uncompressedSize);

			zipFile->Seek(entry.offsetInFile, IReadFile::ESeekOrigin::BEGIN);
			return std::make_unique<MemoryFile>(zipFile, entry.uncompressedSize);
		}

		case 8:		// Deflated
		{
#ifdef DESIRE_ENABLE_ZLIB
			std::unique_ptr<uint8_t[]> compressedData = std::make_unique<uint8_t[]>(entry.compressedSize);
			uint8_t *uncompressedData = (uint8_t*)malloc(entry.uncompressedSize);
			if(compressedData == nullptr || uncompressedData == nullptr)
			{
				free(uncompressedData);
				LOG_ERROR("Not enough memory to decompress file: %s", it->first.c_str());
				return nullptr;
			}

			zipFile->Seek(entry.offsetInFile, IReadFile::ESeekOrigin::BEGIN);
			zipFile->ReadBuffer(compressedData.get(), entry.compressedSize);

			// Setup the inflate stream
			z_stream stream = {};
			stream.next_in = compressedData.get();
			stream.avail_in = entry.compressedSize;
			stream.next_out = uncompressedData;
			stream.avail_out = entry.uncompressedSize;
			stream.opaque = &IAllocator::GetDefaultAllocator();
			DESIRE_TODO("Test and use a LinearAllocator");

			stream.zalloc = [](void *opaque, uint32_t items, uint32_t size)
			{
				IAllocator *allocator = static_cast<IAllocator*>(opaque);
				return allocator->Allocate(items * size);
			};

			stream.zfree = [](void *opaque, void *address)
			{
				IAllocator *allocator = static_cast<IAllocator*>(opaque);
				allocator->Deallocate(address);
			};

			// Decompression with windowBits < 0 for raw inflate (no zlib or gzip header)
			int result = inflateInit2(&stream, -MAX_WBITS);
			if(result == Z_OK)
			{
				result = inflate(&stream, Z_FINISH);
				inflateEnd(&stream);
				if(result == Z_STREAM_END)
				{
					return std::make_unique<MemoryFile>(uncompressedData, entry.uncompressedSize);
				}
				else
				{
					LOG_ERROR("Error decompressing: %s (inflate error: %d %s)", it->first.c_str(), result, (stream.msg != nullptr) ? stream.msg : "");
				}
			}
			else
			{
				LOG_ERROR("Error decompressing: %s (inflateInit2 error: %d %s)", it->first.c_str(), result, (stream.msg != nullptr) ? stream.msg : "");
			}

			free(uncompressedData);
#else
			ASSERT(false && "This compression method is only supported when zlib is enabled");
#endif
			break;
		}

		default:
			LOG_ERROR("File has unsupported compression method.", it->first.c_str());
			break;
	}

	return nullptr;
}

void FileSourceZip::ProcessLocalHeaders()
{
	SZipLocalFileHeader header;
	zipFile->ReadBuffer(&header, sizeof(header));
	if(header.signature != ZIP_SIGNATURE_LOCAL_FILE_HEADER || header.filenameLength == 0)
	{
		return;
	}

	// Read filename
	char filename[DESIRE_MAX_PATH_LEN];
	ASSERT(header.filenameLength < sizeof(filename));
	zipFile->ReadBuffer(filename, header.filenameLength);
	filename[header.filenameLength] = '\0';

	// Skip if directory
	const bool isDirectory = (filename[header.filenameLength - 1] == '/');
	if(isDirectory)
	{
		return;
	}

	// Skip extra field
	if(header.extraFieldLength != 0)
	{
		zipFile->Seek(header.extraFieldLength);
	}

	// Read data descriptor
	if(header.flags & SZipLocalFileHeader::FLAG_DATA_DESCRIPTOR)
	{
		zipFile->ReadBuffer(&header.dataDescriptor, sizeof(SZipDataDescriptor));
	}

	SZipFileEntry entry;
	entry.offsetInFile = zipFile->Tell();
	entry.compressedSize = header.dataDescriptor.compressedSize;
	entry.uncompressedSize = header.dataDescriptor.uncompressedSize;
	entry.compressionMethod = header.compressionMethod;

	fileList[ConvertFilename(filename)] = entry;
}

String FileSourceZip::ConvertFilename(const char *filename)
{
	String strFilename;

	if(flags & FileSystem::FILESOURCE_IGNORE_PATH)
	{
		const char *lastSlash = strrchr(filename, '/');
		if(lastSlash != nullptr)
		{
			strFilename = lastSlash + 1;
		}
		else
		{
			strFilename = filename;
		}
	}
	else
	{
		strFilename = filename;
	}

	if(flags & FileSystem::FILESOURCE_IGNORE_CASE)
	{
		strFilename.ToLower();
	}

	return strFilename;
}
