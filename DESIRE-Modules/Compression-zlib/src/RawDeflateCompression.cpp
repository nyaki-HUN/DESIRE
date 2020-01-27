#include "stdafx_zlib.h"
#include "RawDeflateCompression.h"

RawDeflateCompression::RawDeflateCompression()
	: ZlibCompressionBase(-MAX_WBITS)		// windowBits < 0 for raw deflate (no zlib or gzip header)
{
}
