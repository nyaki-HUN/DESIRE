#include "stdafx.h"
#include "Resource/Texture.h"

Texture::Texture(uint16_t width, uint16_t height, EFormat format, uint8_t numMipMaps)
	: renderData(nullptr)
	, width(width)
	, height(height)
	, format(format)
	, numMipMaps(numMipMaps)
{
	ASSERT(format != EFormat::UNKNOWN);
}

Texture::~Texture()
{
	ASSERT(renderData == nullptr);
}
