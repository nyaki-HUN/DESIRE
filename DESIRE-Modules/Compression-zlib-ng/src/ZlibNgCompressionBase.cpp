#include "ZlibNgCompressionBase.h"

#include "Engine/Core/assert.h"
#include "Engine/Core/Log.h"
#include "Engine/Core/Memory/Allocator.h"

#include "zlib.h"

ZlibNgCompressionBase::ZlibNgCompressionBase(int windowBits)
	: windowBits(windowBits)
{
	compressionLevel = Z_DEFAULT_COMPRESSION;
}

size_t ZlibNgCompressionBase::GetMaxCompressionDataBufferSize(size_t dataSize) const
{
	if(dataSize > UINT32_MAX)
	{
		LOG_ERROR("Data is too large");
		return 0;
	}

	z_stream stream = {};
	StreamInit(stream);

	int result = deflateInit2(&stream, compressionLevel, Z_DEFLATED, windowBits, MAX_MEM_LEVEL, Z_DEFAULT_STRATEGY);
	if(result != Z_OK)
	{
		return 0;
	}

	const size_t maxSize = deflateBound(&stream, (uint32_t)dataSize);

	deflateEnd(&stream);

	return maxSize;
}

size_t ZlibNgCompressionBase::CompressBuffer(void* compressedDataBuffer, size_t compressedDataBufferSize, const void* data, size_t dataSize)
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

size_t ZlibNgCompressionBase::GetMaxDecompressionDataBufferSize(const void* compressedData, size_t compressedDataSize) const
{
	ASSERT(false && "Unimplemented");
	return 0;
}

size_t ZlibNgCompressionBase::DecompressBuffer(void* dataBuffer, size_t dataBufferSize, const void* compressedData, size_t compressedDataSize)
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

int ZlibNgCompressionBase::GetMinCompressionLevel() const
{
	return Z_BEST_SPEED;
}

int ZlibNgCompressionBase::GetMaxCompressionLevel() const
{
	return Z_BEST_COMPRESSION;
}

void* ZlibNgCompressionBase::CustomAlloc(void* opaque, uint32_t items, uint32_t size)
{
	Allocator* allocator = static_cast<Allocator*>(opaque);
	return allocator->Alloc(static_cast<size_t>(items) * size);
}

void ZlibNgCompressionBase::CustomFree(void* opaque, void* address)
{
	Allocator* allocator = static_cast<Allocator*>(opaque);
	allocator->Free(address);
}

void ZlibNgCompressionBase::StreamInit(z_stream& stream)
{
	stream.zalloc = &ZlibNgCompressionBase::CustomAlloc;
	stream.zfree = &ZlibNgCompressionBase::CustomFree;
	stream.opaque = &Allocator::GetDefaultAllocator();
	// TODO: Test and use a LinearAllocator
}
