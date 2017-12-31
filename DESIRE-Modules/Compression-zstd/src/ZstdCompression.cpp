#include "stdafx.h"
#include "ZstdCompression.h"

#include "Core/memory/IAllocator.h"

#define ZSTD_STATIC_LINKING_ONLY
#include "zstd.h"

ZstdCompression::ZstdCompression()
{
	compressionLevel = 19;
}

ZstdCompression::~ZstdCompression()
{
	if(cstream != nullptr)
	{
		ZSTD_freeCStream(cstream);
		cstream = nullptr;
	}

	if(dstream != nullptr)
	{
		ZSTD_freeDStream(dstream);
		dstream = nullptr;
	}
}

size_t ZstdCompression::GetMaxCompressionDataBufferSize(size_t dataSize) const
{
	return ZSTD_compressBound(dataSize);
}

size_t ZstdCompression::CompressBuffer(void *compressedDataBuffer, size_t compressedDataBufferSize, const void *data, size_t dataSize)
{
	return ZSTD_compress(compressedDataBuffer, compressedDataBufferSize, data, dataSize, compressionLevel);
}

size_t ZstdCompression::GetMaxDecompressionDataBufferSize(const void *compressedData, size_t compressedDataSize) const
{
	return ZSTD_getFrameContentSize(compressedData, compressedDataSize);
}

size_t ZstdCompression::DecompressBuffer(void *dataBuffer, size_t dataBufferSize, const void *compressedData, size_t compressedDataSize)
{
	return ZSTD_decompress(dataBuffer, dataBufferSize, compressedData, compressedDataSize);
}

void ZstdCompression::InitStreamForCompression()
{
	if(cstream != nullptr || dstream != nullptr)
	{
		ASSERT(false && "Already initialized");
		return;
	}

#if defined(ZSTD_STATIC_LINKING_ONLY)
	ZSTD_customMem customMem = {};
	customMem.customAlloc = &ZstdCompression::CustomAlloc;
	customMem.customFree = &ZstdCompression::CustomFree;
	customMem.opaque = &IAllocator::GetDefaultAllocator();

	cstream = ZSTD_createCStream_advanced(customMem);
#else
	cstream = ZSTD_createCStream();
#endif

	const size_t result = ZSTD_initCStream(cstream, compressionLevel);
	if(ZSTD_isError(result))
	{
		ZSTD_freeCStream(cstream);
		cstream = nullptr;
	}
}

void ZstdCompression::InitStreamForDecompression()
{
	if(cstream != nullptr || dstream != nullptr)
	{
		ASSERT(false && "Already initialized");
		return;
	}

#if defined(ZSTD_STATIC_LINKING_ONLY)
	ZSTD_customMem customMem = {};
	customMem.customAlloc = &ZstdCompression::CustomAlloc;
	customMem.customFree = &ZstdCompression::CustomFree;
	customMem.opaque = &IAllocator::GetDefaultAllocator();

	dstream = ZSTD_createDStream_advanced(customMem);
#else
	dstream = ZSTD_createDStream();
#endif

	const size_t result = ZSTD_initDStream(dstream);
	if(ZSTD_isError(result))
	{
		ZSTD_freeDStream(dstream);
		dstream = nullptr;
	}
}

int ZstdCompression::GetMinCompressionLevel() const
{
	return 1;
}

int ZstdCompression::GetMaxCompressionLevel() const
{
	return ZSTD_maxCLevel();
}

void* ZstdCompression::CustomAlloc(void *opaque, size_t size)
{
	IAllocator *allocator = static_cast<IAllocator*>(opaque);
	return allocator->Allocate(size);
}

void ZstdCompression::CustomFree(void *opaque, void *address)
{
	IAllocator *allocator = static_cast<IAllocator*>(opaque);
	allocator->Deallocate(address);
}
