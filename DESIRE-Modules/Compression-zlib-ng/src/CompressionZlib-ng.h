#pragma once

#include "Compression/ICompression.h"

class CompressionZlib : public ICompression
{
public:
	CompressionZlib();

	size_t Compress(const void *data, size_t dataSize, void *compressedDataBuffer, size_t compressedDataBufferSize) override;
	size_t Decompress(const void *data, size_t dataSize, void *decompressedDataBuffer, size_t decompressedDataBufferSize) override;

private:
	static void* zlib_alloc(void *opaque, uint32_t items, uint32_t size);
	static void zlib_free(void *opaque, void *address);

	int compressionLevel;
};
