#pragma once

#include "ZlibNgCompressionBase.h"

class RawDeflateCompression : public ZlibNgCompressionBase
{
public:
	RawDeflateCompression();
	~RawDeflateCompression() override;
};
