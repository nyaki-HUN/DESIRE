#include "Engine/stdafx.h"
#include "Engine/Resource/Texture.h"
#include "Engine/Render/Render.h"

Texture::Texture(uint16_t width, uint16_t height, EFormat format, uint8_t numMipMaps)
	: width(width)
	, height(height)
	, format(format)
	, numMipMaps(numMipMaps)
{
	ASSERT(format != EFormat::Unknown);
}

Texture::~Texture()
{
	if(renderData != nullptr)
	{
		Modules::Render->Unbind(this);
	}
}

bool Texture::IsDepthFormat() const
{
	return format == Texture::EFormat::D16
		|| format == Texture::EFormat::D24_S8
		|| format == Texture::EFormat::D32;
}
