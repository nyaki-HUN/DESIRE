#pragma once

#include "Compression/Compression.h"

class ZlibNgCompression : public Compression
{
public:
	ZlibNgCompression();
	~ZlibNgCompression() override;

	size_t GetMaxCompressionDataBufferSize(size_t dataSize) const override;
	size_t CompressBuffer(void *compressedDataBuffer, size_t compressedDataBufferSize, const void *data, size_t dataSize) override;

	size_t GetMaxDecompressionDataBufferSize(const void *compressedData, size_t compressedDataSize) const override;
	size_t DecompressBuffer(void *dataBuffer, size_t dataBufferSize, const void *compressedData, size_t compressedDataSize) override;

	int GetMinCompressionLevel() const override;
	int GetMaxCompressionLevel() const override;

private:
	static void* customAlloc(void *opaque, uint32_t items, uint32_t size);
	static void customFree(void *opaque, void *address);
};
