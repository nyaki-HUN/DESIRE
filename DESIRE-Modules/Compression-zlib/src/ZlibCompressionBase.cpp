#include "ZlibCompressionBase.h"

#include "Engine/Core/assert.h"
#include "Engine/Core/Log/Log.h"
#include "Engine/Core/Memory/MemorySystem.h"

#include "zlib.h"

ZlibCompressionBase::ZlibCompressionBase(int windowBits)
	: windowBits(windowBits)
{
	compressionLevel = Z_DEFAULT_COMPRESSION;
}

size_t ZlibCompressionBase::GetMaxCompressionDataBufferSize(size_t dataSize) const
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

size_t ZlibCompressionBase::CompressBuffer(void* compressedDataBuffer, size_t compressedDataBufferSize, const void* data, size_t dataSize)
{
	if(dataSize > UINT32_MAX || compressedDataBufferSize > UINT32_MAX)
	{
		LOG_ERROR("Error compressing - data is too large");
		return 0;
	}

	z_stream stream = {};
	StreamInit(stream);
	stream.next_in = static_cast<const Bytef*>(data);
	stream.avail_in = static_cast<uInt>(dataSize);
	stream.next_out = static_cast<Bytef*>(compressedDataBuffer);
	stream.avail_out = static_cast<uInt>(compressedDataBufferSize);

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

size_t ZlibCompressionBase::GetMaxDecompressionDataBufferSize(const void* compressedData, size_t compressedDataSize) const
{
	ASSERT(false && "Unimplemented");
	return 0;
}

size_t ZlibCompressionBase::DecompressBuffer(void* dataBuffer, size_t dataBufferSize, const void* compressedData, size_t compressedDataSize)
{
	if(compressedDataSize > UINT32_MAX || dataBufferSize > UINT32_MAX)
	{
		LOG_ERROR("Error decompressing - data is too large");
		return 0;
	}

	z_stream stream = {};
	StreamInit(stream);
	stream.next_in = static_cast<const Bytef*>(compressedData);
	stream.avail_in = static_cast<uInt>(compressedDataSize);
	stream.next_out = static_cast<Bytef*>(dataBuffer);
	stream.avail_out = static_cast<uInt>(dataBufferSize);

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

int ZlibCompressionBase::GetMinCompressionLevel() const
{
	return Z_BEST_SPEED;
}

int ZlibCompressionBase::GetMaxCompressionLevel() const
{
	return Z_BEST_COMPRESSION;
}

void* ZlibCompressionBase::MallocWrapper(void* opaque, uint32_t items, uint32_t size)
{
	DESIRE_UNUSED(opaque);
	return MemorySystem::Alloc(static_cast<size_t>(items) * size);
}

void ZlibCompressionBase::FreeWrapper(void* opaque, void* address)
{
	DESIRE_UNUSED(opaque);
	MemorySystem::Free(address);
}

void ZlibCompressionBase::StreamInit(z_stream& stream)
{
	stream.zalloc = &ZlibCompressionBase::MallocWrapper;
	stream.zfree = &ZlibCompressionBase::FreeWrapper;
}
