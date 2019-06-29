#include "ZstdCompression.h"

#include "Engine/Core/assert.h"
#include "Engine/Core/Memory/MemorySystem.h"

#define ZSTD_STATIC_LINKING_ONLY
#include "zstd.h"

ZstdCompression::ZstdCompression()
{
	compressionLevel = 19;
}

size_t ZstdCompression::GetMaxCompressionDataBufferSize(size_t dataSize) const
{
	return ZSTD_compressBound(dataSize);
}

size_t ZstdCompression::CompressBuffer(void* compressedDataBuffer, size_t compressedDataBufferSize, const void* data, size_t dataSize)
{
	return ZSTD_compress(compressedDataBuffer, compressedDataBufferSize, data, dataSize, compressionLevel);
}

size_t ZstdCompression::GetMaxDecompressionDataBufferSize(const void* compressedData, size_t compressedDataSize) const
{
	return ZSTD_getFrameContentSize(compressedData, compressedDataSize);
}

size_t ZstdCompression::DecompressBuffer(void* dataBuffer, size_t dataBufferSize, const void* compressedData, size_t compressedDataSize)
{
	return ZSTD_decompress(dataBuffer, dataBufferSize, compressedData, compressedDataSize);
}

int ZstdCompression::GetMinCompressionLevel() const
{
	return 1;
}

int ZstdCompression::GetMaxCompressionLevel() const
{
	return ZSTD_maxCLevel();
}

void* ZstdCompression::CustomAlloc(void* opaque, size_t size)
{
	DESIRE_UNUSED(opaque);
	return MemorySystem::Alloc(size);
}

void ZstdCompression::CustomFree(void* opaque, void* address)
{
	DESIRE_UNUSED(opaque);
	MemorySystem::Free(address);
}
