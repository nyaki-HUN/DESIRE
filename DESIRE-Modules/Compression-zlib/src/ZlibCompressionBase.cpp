#include "stdafx_zlib.h"
#include "ZlibCompressionBase.h"

#include "Engine/Core/Memory/MemorySystem.h"

ZlibCompressionBase::ZlibCompressionBase(int32_t windowBits)
	: Compression(Z_DEFAULT_COMPRESSION)
	, m_windowBits(windowBits)
{
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

	int32_t result = deflateInit2(&stream, m_compressionLevel, Z_DEFLATED, m_windowBits, MAX_MEM_LEVEL, Z_DEFAULT_STRATEGY);
	if(result != Z_OK)
	{
		return 0;
	}

	const size_t maxSize = deflateBound(&stream, (uint32_t)dataSize);

	deflateEnd(&stream);

	return maxSize;
}

size_t ZlibCompressionBase::CompressBuffer(void* pCompressedDataBuffer, size_t compressedDataBufferSize, const void* pData, size_t dataSize)
{
	if(dataSize > UINT32_MAX || compressedDataBufferSize > UINT32_MAX)
	{
		LOG_ERROR("Error compressing - data is too large");
		return 0;
	}

	z_stream stream = {};
	StreamInit(stream);
	stream.next_in = static_cast<const Bytef*>(pData);
	stream.avail_in = static_cast<uInt>(dataSize);
	stream.next_out = static_cast<Bytef*>(pCompressedDataBuffer);
	stream.avail_out = static_cast<uInt>(compressedDataBufferSize);

	int32_t result = deflateInit2(&stream, m_compressionLevel, Z_DEFLATED, m_windowBits, MAX_MEM_LEVEL, Z_DEFAULT_STRATEGY);
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

size_t ZlibCompressionBase::GetMaxDecompressionDataBufferSize(const void* pCompressedData, size_t pCompressedDataSize) const
{
	ASSERT(false && "Unimplemented");
	return 0;
}

size_t ZlibCompressionBase::DecompressBuffer(void* pDataBuffer, size_t dataBufferSize, const void* pCompressedData, size_t compressedDataSize)
{
	if(compressedDataSize > UINT32_MAX || dataBufferSize > UINT32_MAX)
	{
		LOG_ERROR("Error decompressing - data is too large");
		return 0;
	}

	z_stream stream = {};
	StreamInit(stream);
	stream.next_in = static_cast<const Bytef*>(pCompressedData);
	stream.avail_in = static_cast<uInt>(compressedDataSize);
	stream.next_out = static_cast<Bytef*>(pDataBuffer);
	stream.avail_out = static_cast<uInt>(dataBufferSize);

	int32_t result = inflateInit2(&stream, m_windowBits);
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

int32_t ZlibCompressionBase::GetMinCompressionLevel() const
{
	return Z_BEST_SPEED;
}

int32_t ZlibCompressionBase::GetMaxCompressionLevel() const
{
	return Z_BEST_COMPRESSION;
}

void* ZlibCompressionBase::MallocWrapper(void* pOpaque, uint32_t items, uint32_t size)
{
	DESIRE_UNUSED(pOpaque);
	return MemorySystem::Alloc(static_cast<size_t>(items) * size);
}

void ZlibCompressionBase::FreeWrapper(void* pOpaque, void* pAddress)
{
	DESIRE_UNUSED(pOpaque);
	MemorySystem::Free(pAddress);
}

void ZlibCompressionBase::StreamInit(z_stream& stream)
{
	stream.zalloc = &ZlibCompressionBase::MallocWrapper;
	stream.zfree = &ZlibCompressionBase::FreeWrapper;
}
