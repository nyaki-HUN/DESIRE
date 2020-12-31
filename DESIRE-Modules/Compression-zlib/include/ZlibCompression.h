#pragma once

#include "ZlibCompressionBase.h"

class ZlibCompression : public ZlibCompressionBase
{
public:
	ZlibCompression();

	size_t GetMaxCompressionDataBufferSize(size_t dataSize) const override;
	size_t CompressBuffer(void* pCompressedDataBuffer, size_t compressedDataBufferSize, const void* pData, size_t dataSize) override;

	size_t DecompressBuffer(void* pDataBuffer, size_t dataBufferSize, const void* pCompressedData, size_t compressedDataSize) override;
};
