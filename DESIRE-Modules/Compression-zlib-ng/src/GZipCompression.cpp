#include "stdafx.h"
#include "GZipCompression.h"

#include "zlib.h"

GZipCompression::GZipCompression()
	: ZlibNgCompressionBase(MAX_WBITS + 16)		// use gzip header
{

}

GZipCompression::~GZipCompression()
{

}
