#pragma once

#include "Engine/Compression/Compression.h"

typedef struct ZSTD_CCtx_s ZSTD_CCtx;
typedef struct ZSTD_DCtx_s ZSTD_DCtx;

class ZstdCompression : public Compression
{
public:
	ZstdCompression();
	~ZstdCompression();

	size_t GetMaxCompressionDataBufferSize(size_t dataSize) const override;
	size_t CompressBuffer(void* pCompressedDataBuffer, size_t compressedDataBufferSize, const void* pData, size_t dataSize) override;

	size_t GetMaxDecompressionDataBufferSize(const void* pCompressedData, size_t compressedDataSize) const override;
	size_t DecompressBuffer(void* pDataBuffer, size_t dataBufferSize, const void* pCompressedData, size_t compressedDataSize) override;

	int32_t GetMinCompressionLevel() const override;
	int32_t GetMaxCompressionLevel() const override;

private:
	static void* MallocWrapper(void* pOpaque, size_t size);
	static void FreeWrapper(void* pOpaque, void* pAddress);

	ZSTD_CCtx* m_pCompressContext = nullptr;
	ZSTD_DCtx* m_pDecompressContext = nullptr;
};
