#pragma once

class Compression
{
protected:
	Compression();

public:
	virtual ~Compression();

	// Simple compress
	virtual size_t GetMaxCompressionDataBufferSize(size_t dataSize) const = 0;
	virtual size_t CompressBuffer(void *compressedDataBuffer, size_t compressedDataBufferSize, const void *data, size_t dataSize) = 0;

	// Simple decompress
	virtual size_t GetMaxDecompressionDataBufferSize(const void *compressedData, size_t compressedDataSize) const = 0;
	virtual size_t DecompressBuffer(void *dataBuffer, size_t dataBufferSize, const void *compressedData, size_t compressedDataSize) = 0;

	// Options
	void SetCompressionLevel(int level);
	virtual int GetMinCompressionLevel() const;
	virtual int GetMaxCompressionLevel() const;

	// Stream API for compress/decompress
	virtual void InitStreamForCompression();
	virtual void InitStreamForDecompression();
	virtual void ProcessStream();

protected:
	int compressionLevel = 0;
};
