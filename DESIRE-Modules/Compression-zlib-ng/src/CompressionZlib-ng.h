#pragma once

#include "Compression/ICompression.h"

class CompressionZlib : public ICompression
{
public:
	CompressionZlib();

	size_t Compress(const void *data, size_t dataSize, void *compressedDataBuffer, size_t compressedDataBufferSize) override;
	size_t Decompress(const void *data, size_t dataSize, void *decompressedDataBuffer, size_t decompressedDataBufferSize) override;

private:
	int compressionLevel;
};
