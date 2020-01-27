#include "stdafx_zstd.h"
#include "ZstdCompression.h"

#include "Engine/Core/Memory/MemorySystem.h"

ZstdCompression::ZstdCompression()
	: Compression(19)
{
}

ZstdCompression::~ZstdCompression()
{
	if(compressContext != nullptr)
	{
		ZSTD_freeCCtx(compressContext);
		compressContext = nullptr;
	}

	if(decompressContext != nullptr)
	{
		ZSTD_freeDCtx(decompressContext);
		decompressContext = nullptr;
	}
}

size_t ZstdCompression::GetMaxCompressionDataBufferSize(size_t dataSize) const
{
	return ZSTD_compressBound(dataSize);
}

size_t ZstdCompression::CompressBuffer(void* compressedDataBuffer, size_t compressedDataBufferSize, const void* data, size_t dataSize)
{
	if(compressContext == nullptr)
	{
		const ZSTD_customMem customMem =
		{
			&ZstdCompression::MallocWrapper,
			&ZstdCompression::FreeWrapper,
			this
		};

		compressContext = ZSTD_createCCtx_advanced(customMem);
	}

	return ZSTD_compressCCtx(compressContext, compressedDataBuffer, compressedDataBufferSize, data, dataSize, compressionLevel);
}

size_t ZstdCompression::GetMaxDecompressionDataBufferSize(const void* compressedData, size_t compressedDataSize) const
{
	return ZSTD_getFrameContentSize(compressedData, compressedDataSize);
}

size_t ZstdCompression::DecompressBuffer(void* dataBuffer, size_t dataBufferSize, const void* compressedData, size_t compressedDataSize)
{
	if(decompressContext == nullptr)
	{
		const ZSTD_customMem customMem =
		{
			&ZstdCompression::MallocWrapper,
			&ZstdCompression::FreeWrapper,
			this
		};

		decompressContext = ZSTD_createDCtx_advanced(customMem);
	}

	return ZSTD_decompressDCtx(decompressContext, dataBuffer, dataBufferSize, compressedData, compressedDataSize);
}

int ZstdCompression::GetMinCompressionLevel() const
{
	return ZSTD_minCLevel();
}

int ZstdCompression::GetMaxCompressionLevel() const
{
	return ZSTD_maxCLevel();
}

void* ZstdCompression::MallocWrapper(void* opaque, size_t size)
{
	DESIRE_UNUSED(opaque);
	return MemorySystem::Alloc(size);
}

void ZstdCompression::FreeWrapper(void* opaque, void* address)
{
	DESIRE_UNUSED(opaque);
	MemorySystem::Free(address);
}
