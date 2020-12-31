#include "stdafx_zlib.h"
#include "ZlibCompression.h"

ZlibCompression::ZlibCompression()
	: ZlibCompressionBase(MAX_WBITS)		// use zlib header
{
}

size_t ZlibCompression::GetMaxCompressionDataBufferSize(size_t dataSize) const
{
	return compressBound(static_cast<uLong>(dataSize));
}

size_t ZlibCompression::CompressBuffer(void* pCompressedDataBuffer, size_t compressedDataBufferSize, const void* pData, size_t dataSize)
{
	uLongf compressedSize = static_cast<unsigned long>(compressedDataBufferSize);
	const int32_t result = compress2(static_cast<Bytef*>(pCompressedDataBuffer), &compressedSize, static_cast<const Bytef*>(pData), static_cast<uLong>(dataSize), m_compressionLevel);
	if(result != Z_OK)
	{
		LOG_ERROR("Error compressing - compress2 error: %d", result);
	}

	return compressedSize;
}

size_t ZlibCompression::DecompressBuffer(void* pDataBuffer, size_t dataBufferSize, const void* pCompressedData, size_t compressedDataSize)
{
	uLongf uncompressedSize = static_cast<uLongf>(dataBufferSize);
	const int32_t result = uncompress(static_cast<Bytef*>(pDataBuffer), &uncompressedSize, static_cast<const Bytef*>(pCompressedData), static_cast<uLong>(compressedDataSize));
	if(result != Z_OK)
	{
		LOG_ERROR("Error compressing - uncompress error: %d", result);
	}

	return uncompressedSize;
}
