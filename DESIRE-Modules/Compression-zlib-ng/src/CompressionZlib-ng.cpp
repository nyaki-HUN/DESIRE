#include "stdafx.h"
#include "CompressionZlib-ng.h"

#include "Core/memory/IAllocator.h"

#include "zlib.h"

CompressionZlib::CompressionZlib()
{
	compressionLevel = Z_DEFAULT_COMPRESSION;
}

size_t CompressionZlib::Compress(const void *data, size_t dataSize, void *compressedDataBuffer, size_t compressedDataBufferSize)
{
	if(dataSize > UINT32_MAX || compressedDataBufferSize > UINT32_MAX)
	{
		LOG_ERROR("Error compressing - data is too large");
		return 0;
	}

	z_stream stream = {};
	stream.next_in = static_cast<const uint8_t*>(data);
	stream.avail_in = (uint32_t)dataSize;
	stream.next_out = static_cast<uint8_t*>(compressedDataBuffer);
	stream.avail_out = (uint32_t)compressedDataBufferSize;
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

	// Compression with windowBits < 0 for raw deflate (no zlib or gzip header)
	int result = deflateInit2(&stream, compressionLevel, Z_DEFLATED, -MAX_WBITS, MAX_MEM_LEVEL, Z_DEFAULT_STRATEGY);
	if(result != Z_OK)
	{
		LOG_ERROR("Error compressing - deflateInit2 error: %d %s", result, (stream.msg != nullptr) ? stream.msg : "");
		return 0;
	}

	result = deflate(&stream, Z_FINISH);
	if(result != Z_STREAM_END)
	{
		LOG_ERROR("Error compressing - deflate error: %d %s)", result, (stream.msg != nullptr) ? stream.msg : "");
		return 0;
	}

	deflateEnd(&stream);

	return stream.total_out;
}

size_t CompressionZlib::Decompress(const void *data, size_t dataSize, void *decompressedDataBuffer, size_t decompressedDataBufferSize)
{
	if(dataSize > UINT32_MAX || decompressedDataBufferSize > UINT32_MAX)
	{
		LOG_ERROR("Error decompressing - data is too large");
		return 0;
	}

	z_stream stream = {};
	stream.next_in = static_cast<const uint8_t*>(data);
	stream.avail_in = (uint32_t)dataSize;
	stream.next_out = static_cast<uint8_t*>(decompressedDataBuffer);
	stream.avail_out = (uint32_t)decompressedDataBufferSize;
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
