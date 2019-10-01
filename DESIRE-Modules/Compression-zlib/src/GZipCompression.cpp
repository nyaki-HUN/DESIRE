#include "GZipCompression.h"

#include "zlib.h"

GZipCompression::GZipCompression()
	: ZlibCompressionBase(MAX_WBITS + 16)		// use gzip header
{
}

GZipCompression::~GZipCompression()
{
}
