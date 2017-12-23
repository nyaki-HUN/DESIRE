#pragma once

#include "Compression/ICompression.h"

class CompressionZlib : public ICompression
{
public:
	CompressionZlib();

	size_t Compress(const void *data, size_t dataSize, void *compressedData, size_t compressedDataSize) override;
	size_t Decompress(const void *compressedData, size_t compressedDataSize, void *decompressedData, size_t decompressedDataSize) override;

private:
	int compressionLevel;
};
