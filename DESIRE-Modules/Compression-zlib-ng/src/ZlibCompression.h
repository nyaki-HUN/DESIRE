#pragma once

#include "ZlibNgCompressionBase.h"

class ZlibCompression : public ZlibNgCompressionBase
{
public:
	ZlibCompression();
	~ZlibCompression() override;

	size_t GetMaxCompressionDataBufferSize(size_t dataSize) const override;
	size_t CompressBuffer(void *compressedDataBuffer, size_t compressedDataBufferSize, const void *data, size_t dataSize) override;

	size_t DecompressBuffer(void *dataBuffer, size_t dataBufferSize, const void *compressedData, size_t compressedDataSize) override;
};
