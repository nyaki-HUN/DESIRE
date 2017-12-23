#pragma once

class ICompression
{
protected:
	ICompression() {};

public:
	virtual ~ICompression() {};

	virtual size_t Compress(const void *data, size_t dataSize, void *compressedData, size_t compressedDataSize) = 0;
	virtual size_t Decompress(const void *compressedData, size_t compressedDataSize, void *decompressedData, size_t decompressedDataSize) = 0;
};
