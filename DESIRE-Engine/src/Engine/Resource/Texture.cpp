#include "Engine/stdafx.h"
#include "Engine/Resource/Texture.h"
#include "Engine/Render/Render.h"

Texture::Texture(uint16_t width, uint16_t height, EFormat format, uint8_t numMipMaps)
	: width(width)
	, height(height)
	, format(format)
	, numMipMaps(numMipMaps)
{
	ASSERT(format != EFormat::UNKNOWN);
}

Texture::~Texture()
{
	if(renderData != nullptr)
	{
		Render::Get()->Unbind(this);
	}
}

bool Texture::IsDepthFormat() const
{
	return (format == Texture::EFormat::D24S8);
}
