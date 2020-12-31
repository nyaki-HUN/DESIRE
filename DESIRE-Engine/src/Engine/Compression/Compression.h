#pragma once

class Compression
{
protected:
	Compression(int32_t defaultCompressionLevel);

public:
	virtual ~Compression();

	// Simple compress
	virtual size_t GetMaxCompressionDataBufferSize(size_t dataSize) const = 0;
	virtual size_t CompressBuffer(void* pCompressedDataBuffer, size_t compressedDataBufferSize, const void* pData, size_t dataSize) = 0;

	// Simple decompress
	virtual size_t GetMaxDecompressionDataBufferSize(const void* pCompressedData, size_t compressedDataSize) const = 0;
	virtual size_t DecompressBuffer(void* pDataBuffer, size_t dataBufferSize, const void* pCompressedData, size_t compressedDataSize) = 0;

	// Options
	void SetCompressionLevel(int32_t level);
	virtual int32_t GetMinCompressionLevel() const;
	virtual int32_t GetMaxCompressionLevel() const;

protected:
	int32_t m_compressionLevel = 0;
};
