#pragma once

#include "Engine/Compression/Compression.h"

typedef struct ZSTD_CCtx_s ZSTD_CCtx;
typedef struct ZSTD_DCtx_s ZSTD_DCtx;

class ZstdCompression : public Compression
{
public:
	ZstdCompression();

	size_t GetMaxCompressionDataBufferSize(size_t dataSize) const override;
	size_t CompressBuffer(void *compressedDataBuffer, size_t compressedDataBufferSize, const void *data, size_t dataSize) override;

	size_t GetMaxDecompressionDataBufferSize(const void *compressedData, size_t compressedDataSize) const override;
	size_t DecompressBuffer(void *dataBuffer, size_t dataBufferSize, const void *compressedData, size_t compressedDataSize) override;

	int GetMinCompressionLevel() const override;
	int GetMaxCompressionLevel() const override;

private:
	static void* MallocWrapper(void *opaque, size_t size);
	static void FreeWrapper(void *opaque, void *address);

	ZSTD_CCtx* compressContext = nullptr;
	ZSTD_DCtx* decompressContext = nullptr;
};
