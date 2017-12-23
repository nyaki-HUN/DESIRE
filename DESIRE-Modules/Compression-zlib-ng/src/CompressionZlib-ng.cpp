#include "stdafx.h"
#include "CompressionZlib-ng.h"

#include "Core/memory/IAllocator.h"

#include "zlib.h"

CompressionZlib::CompressionZlib()
{
	compressionLevel = Z_DEFAULT_COMPRESSION;
}

size_t CompressionZlib::Compress(const void *data, size_t dataSize, void *compressedData, size_t compressedDataSize)
{
	DESIRE_TODO("Implement");
	return 0;
}

size_t CompressionZlib::Decompress(const void *compressedData, size_t compressedDataSize, void *decompressedData, size_t decompressedDataSize)
{
	z_stream stream = {};
	stream.next_in = static_cast<const uint8_t*>(compressedData);
	stream.avail_in = (uint32_t)compressedDataSize;
	stream.next_out = static_cast<uint8_t*>(decompressedData);
	stream.avail_out = (uint32_t)decompressedDataSize;
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
	if(result != Z_OK)
	{
		LOG_ERROR("Error decompressing - inflateInit2 error: %d %s", result, (stream.msg != nullptr) ? stream.msg : "");
		return 0;
	}

	result = inflate(&stream, Z_FINISH);
	if(result != Z_STREAM_END)
	{
		LOG_ERROR("Error decompressing - inflate error: %d %s)", result, (stream.msg != nullptr) ? stream.msg : "");
		return 0;
	}

	inflateEnd(&stream);

	return stream.total_out;
}
