#pragma once

class ICompression
{
protected:
	ICompression() {}

public:
	virtual ~ICompression() {}

	virtual size_t Compress(const void *data, size_t dataSize, void *compressedDataBuffer, size_t compressedDataBufferSize) = 0;
	virtual size_t Decompress(const void *data, size_t dataSize, void *decompressedDataBuffer, size_t decompressedDataBufferSize) = 0;
};
