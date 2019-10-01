#include "ZlibCompression.h"

#include "Engine/Core/Log/Log.h"

#include "zlib.h"

ZlibCompression::ZlibCompression()
	: ZlibCompressionBase(MAX_WBITS)		// use zlib header
{
}

ZlibCompression::~ZlibCompression()
{
}

size_t ZlibCompression::GetMaxCompressionDataBufferSize(size_t dataSize) const
{
	return compressBound(static_cast<uLong>(dataSize));
}

size_t ZlibCompression::CompressBuffer(void* compressedDataBuffer, size_t compressedDataBufferSize, const void* data, size_t dataSize)
{
	uLongf compressedSize = static_cast<unsigned long>(compressedDataBufferSize);
	const int result = compress2(static_cast<Bytef*>(compressedDataBuffer), &compressedSize, static_cast<const Bytef*>(data), static_cast<uLong>(dataSize), compressionLevel);
	if(result != Z_OK)
	{
		LOG_ERROR("Error compressing - compress2 error: %d", result);
	}

	return compressedSize;
}

size_t ZlibCompression::DecompressBuffer(void* dataBuffer, size_t dataBufferSize, const void* compressedData, size_t compressedDataSize)
{
	uLongf uncompressedSize = static_cast<uLongf>(dataBufferSize);
	const int result = uncompress(static_cast<Bytef*>(dataBuffer), &uncompressedSize, static_cast<const Bytef*>(compressedData), static_cast<uLong>(compressedDataSize));
	if(result != Z_OK)
	{
		LOG_ERROR("Error compressing - uncompress error: %d", result);
	}

	return uncompressedSize;
}
