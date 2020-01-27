#include "stdafx_zlib.h"
#include "GZipCompression.h"

GZipCompression::GZipCompression()
	: ZlibCompressionBase(MAX_WBITS + 16)		// use gzip header
{
}
