#include "stdafx_zstd.h"
#include "ZstdCompression.h"

#include "Engine/Core/Memory/MemorySystem.h"

ZstdCompression::ZstdCompression()
	: Compression(19)
{
}

ZstdCompression::~ZstdCompression()
{
	if(m_pCompressContext != nullptr)
	{
		ZSTD_freeCCtx(m_pCompressContext);
		m_pCompressContext = nullptr;
	}

	if(m_pDecompressContext != nullptr)
	{
		ZSTD_freeDCtx(m_pDecompressContext);
		m_pDecompressContext = nullptr;
	}
}

size_t ZstdCompression::GetMaxCompressionDataBufferSize(size_t dataSize) const
{
	return ZSTD_compressBound(dataSize);
}

size_t ZstdCompression::CompressBuffer(void* pCompressedDataBuffer, size_t compressedDataBufferSize, const void* pData, size_t dataSize)
{
	if(m_pCompressContext == nullptr)
	{
		const ZSTD_customMem customMem =
		{
			&ZstdCompression::MallocWrapper,
			&ZstdCompression::FreeWrapper,
			this
		};

		m_pCompressContext = ZSTD_createCCtx_advanced(customMem);
	}

	return ZSTD_compressCCtx(m_pCompressContext, pCompressedDataBuffer, compressedDataBufferSize, pData, dataSize, m_compressionLevel);
}

size_t ZstdCompression::GetMaxDecompressionDataBufferSize(const void* compressedData, size_t compressedDataSize) const
{
	return ZSTD_getFrameContentSize(compressedData, compressedDataSize);
}

size_t ZstdCompression::DecompressBuffer(void* pDataBuffer, size_t dataBufferSize, const void* pCompressedData, size_t compressedDataSize)
{
	if(m_pDecompressContext == nullptr)
	{
		const ZSTD_customMem customMem =
		{
			&ZstdCompression::MallocWrapper,
			&ZstdCompression::FreeWrapper,
			this
		};

		m_pDecompressContext = ZSTD_createDCtx_advanced(customMem);
	}

	return ZSTD_decompressDCtx(m_pDecompressContext, pDataBuffer, dataBufferSize, pCompressedData, compressedDataSize);
}

int32_t ZstdCompression::GetMinCompressionLevel() const
{
	return ZSTD_minCLevel();
}

int32_t ZstdCompression::GetMaxCompressionLevel() const
{
	return ZSTD_maxCLevel();
}

void* ZstdCompression::MallocWrapper(void* pOpaque, size_t size)
{
	DESIRE_UNUSED(pOpaque);
	return MemorySystem::Alloc(size);
}

void ZstdCompression::FreeWrapper(void* pOpaque, void* pAddress)
{
	DESIRE_UNUSED(pOpaque);
	MemorySystem::Free(pAddress);
}
