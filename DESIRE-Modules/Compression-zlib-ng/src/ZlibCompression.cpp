#include "ZlibCompression.h"

#include "Engine/Core/Log.h"

#include "zlib.h"

ZlibCompression::ZlibCompression()
	: ZlibNgCompressionBase(MAX_WBITS)		// use zlib header
{

}

ZlibCompression::~ZlibCompression()
{

}

size_t ZlibCompression::GetMaxCompressionDataBufferSize(size_t dataSize) const
{
	return compressBound(static_cast<unsigned long>(dataSize));
}

size_t ZlibCompression::CompressBuffer(void *compressedDataBuffer, size_t compressedDataBufferSize, const void *data, size_t dataSize)
{
	unsigned long compressedSize = static_cast<unsigned long>(compressedDataBufferSize);
	const int result = compress2(static_cast<uint8_t*>(compressedDataBuffer), &compressedSize, static_cast<const uint8_t*>(data), static_cast<unsigned long>(dataSize), compressionLevel);
	if(result != Z_OK)
	{
		LOG_ERROR("Error compressing - compress2 error: %d", result);
	}

	return compressedSize;
}

size_t ZlibCompression::DecompressBuffer(void *dataBuffer, size_t dataBufferSize, const void *compressedData, size_t compressedDataSize)
{
	unsigned long uncompressedSize = static_cast<unsigned long>(dataBufferSize);
	const int result = uncompress(static_cast<uint8_t*>(dataBuffer), &uncompressedSize, static_cast<const uint8_t*>(compressedData), static_cast<unsigned long>(compressedDataSize));
	if(result != Z_OK)
	{
		LOG_ERROR("Error compressing - uncompress error: %d", result);
	}

	return uncompressedSize;
}
