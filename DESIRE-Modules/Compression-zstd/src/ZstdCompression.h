#pragma once

#include "Compression/Compression.h"

typedef struct ZSTD_CCtx_s ZSTD_CStream;
typedef struct ZSTD_DCtx_s ZSTD_DStream;

class ZstdCompression : public Compression
{
public:
	ZstdCompression();
	~ZstdCompression() override;

	size_t GetMaxCompressionDataBufferSize(size_t dataSize) const override;
	size_t CompressBuffer(void *compressedDataBuffer, size_t compressedDataBufferSize, const void *data, size_t dataSize) override;

	size_t GetMaxDecompressionDataBufferSize(const void *compressedData, size_t compressedDataSize) const override;
	size_t DecompressBuffer(void *dataBuffer, size_t dataBufferSize, const void *compressedData, size_t compressedDataSize) override;

	void InitStreamForCompression()override;
	void InitStreamForDecompression() override;

	int GetMinCompressionLevel() const override;
	int GetMaxCompressionLevel() const override;

private:
	static void* CustomAlloc(void *opaque, size_t size);
	static void CustomFree(void *opaque, void *address);

	ZSTD_CStream *cstream = nullptr;
	ZSTD_DStream *dstream = nullptr;
};
