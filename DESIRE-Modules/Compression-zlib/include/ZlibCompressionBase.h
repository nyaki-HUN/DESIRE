#pragma once

#include "Engine/Compression/Compression.h"

typedef struct z_stream_s z_stream;

class ZlibCompressionBase : public Compression
{
public:
	ZlibCompressionBase(int32_t windowBits);

	size_t GetMaxCompressionDataBufferSize(size_t dataSize) const override;
	size_t CompressBuffer(void* pCompressedDataBuffer, size_t compressedDataBufferSize, const void* pData, size_t dataSize) override;

	size_t GetMaxDecompressionDataBufferSize(const void* pCompressedData, size_t compressedDataSize) const override;
	size_t DecompressBuffer(void* pDataBuffer, size_t dataBufferSize, const void* pCompressedData, size_t compressedDataSize) override;

	int32_t GetMinCompressionLevel() const override;
	int32_t GetMaxCompressionLevel() const override;

protected:
	static void* MallocWrapper(void* pOpaque, uint32_t items, uint32_t size);
	static void FreeWrapper(void* pOpaque, void* pAddress);
	static void StreamInit(z_stream& stream);

	const int32_t m_windowBits;
};
