#include "RawDeflateCompression.h"

#include "zlib.h"

RawDeflateCompression::RawDeflateCompression()
	: ZlibNgCompressionBase(-MAX_WBITS)		// windowBits < 0 for raw deflate (no zlib or gzip header)
{

}

RawDeflateCompression::~RawDeflateCompression()
{

}
