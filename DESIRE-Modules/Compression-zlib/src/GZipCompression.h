#pragma once

#include "ZlibNgCompressionBase.h"

class GZipCompression : public ZlibNgCompressionBase
{
public:
	GZipCompression();
	~GZipCompression() override;

private:
};
