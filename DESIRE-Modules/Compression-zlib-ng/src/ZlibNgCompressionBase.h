#pragma once

#include "Engine/Compression/Compression.h"

#include <stdint.h>

typedef struct z_stream_s z_stream;

class ZlibNgCompressionBase : public Compression
{
public:
	ZlibNgCompressionBase(int windowBits);
	~ZlibNgCompressionBase() override;

	size_t GetMaxCompressionDataBufferSize(size_t dataSize) const override;
	size_t CompressBuffer(void *compressedDataBuffer, size_t compressedDataBufferSize, const void *data, size_t dataSize) override;

	size_t GetMaxDecompressionDataBufferSize(const void *compressedData, size_t compressedDataSize) const override;
	size_t DecompressBuffer(void *dataBuffer, size_t dataBufferSize, const void *compressedData, size_t compressedDataSize) override;

	int GetMinCompressionLevel() const override;
	int GetMaxCompressionLevel() const override;

	void InitStreamForCompression() override;
	void InitStreamForDecompression() override;

protected:
	static void* CustomAlloc(void *opaque, uint32_t items, uint32_t size);
	static void CustomFree(void *opaque, void *address);
	static void StreamInit(z_stream& stream);

	const int windowBits;
	z_stream *deflateStream = nullptr;
	z_stream *inflateStream = nullptr;
};
