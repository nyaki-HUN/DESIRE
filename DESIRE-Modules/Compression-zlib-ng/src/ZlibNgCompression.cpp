#include "stdafx.h"
#include "ZlibNgCompression.h"

#include "Core/memory/IAllocator.h"

#include "zlib.h"

ZlibNgCompression::ZlibNgCompression()
{
	compressionLevel = Z_DEFAULT_COMPRESSION;
}

ZlibNgCompression::~ZlibNgCompression()
{
	if(deflateStream != nullptr)
	{
		deflateEnd(deflateStream);
		delete deflateStream;
		deflateStream = nullptr;
	}

	if(inflateStream != nullptr)
	{
		inflateEnd(inflateStream);
		delete inflateStream;
		inflateStream = nullptr;
	}
}

size_t ZlibNgCompression::GetMaxCompressionDataBufferSize(size_t dataSize) const
{
	ASSERT(false && "Unimplemented");
	return 0;
}

size_t ZlibNgCompression::CompressBuffer(void *compressedDataBuffer, size_t compressedDataBufferSize, const void *data, size_t dataSize)
{
	if(dataSize > UINT32_MAX || compressedDataBufferSize > UINT32_MAX)
	{
		LOG_ERROR("Error compressing - data is too large");
		return 0;
	}

	z_stream stream = {};
	StreamInit(stream);
	stream.next_in = static_cast<const uint8_t*>(data);
	stream.avail_in = (uint32_t)dataSize;
	stream.next_out = static_cast<uint8_t*>(compressedDataBuffer);
	stream.avail_out = (uint32_t)compressedDataBufferSize;

	// Compression with windowBits < 0 for raw deflate (no zlib or gzip header)
	const int windowBits = -MAX_WBITS;
	int result = deflateInit2(&stream, compressionLevel, Z_DEFLATED, windowBits, MAX_MEM_LEVEL, Z_DEFAULT_STRATEGY);
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

size_t ZlibNgCompression::GetMaxDecompressionDataBufferSize(const void *compressedData, size_t compressedDataSize) const
{
	ASSERT(false && "Unimplemented");
	return 0;
}

size_t ZlibNgCompression::DecompressBuffer(void *dataBuffer, size_t dataBufferSize, const void *compressedData, size_t compressedDataSize)
{
	if(compressedDataSize > UINT32_MAX || dataBufferSize > UINT32_MAX)
	{
		LOG_ERROR("Error decompressing - data is too large");
		return 0;
	}

	z_stream stream = {};
	StreamInit(stream);
	stream.next_in = static_cast<const uint8_t*>(compressedData);
	stream.avail_in = (uint32_t)compressedDataSize;
	stream.next_out = static_cast<uint8_t*>(dataBuffer);
	stream.avail_out = (uint32_t)dataBufferSize;

	// Decompression with windowBits < 0 for raw inflate (no zlib or gzip header)
	const int windowBits = -MAX_WBITS;
	int result = inflateInit2(&stream, windowBits);
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

int ZlibNgCompression::GetMinCompressionLevel() const
{
	return Z_BEST_SPEED;
}

int ZlibNgCompression::GetMaxCompressionLevel() const
{
	return Z_BEST_COMPRESSION;
}

void* ZlibNgCompression::CustomAlloc(void *opaque, uint32_t items, uint32_t size)
{
	IAllocator *allocator = static_cast<IAllocator*>(opaque);
	return allocator->Allocate(items * size);
}

void ZlibNgCompression::CustomFree(void *opaque, void *address)
{
	IAllocator *allocator = static_cast<IAllocator*>(opaque);
	allocator->Deallocate(address);
}

void ZlibNgCompression::StreamInit(z_stream& stream)
{
	stream.zalloc = &ZlibNgCompression::CustomAlloc;
	stream.zfree = &ZlibNgCompression::CustomFree;
	stream.opaque = &IAllocator::GetDefaultAllocator();
	DESIRE_TODO("Test and use a LinearAllocator");
}
