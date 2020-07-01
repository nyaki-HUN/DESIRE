#include "Engine/stdafx.h"
#include "Engine/Resource/Texture.h"

#include "Engine/Render/Render.h"

Texture::Texture(uint16_t width, uint16_t height, EFormat format, std::unique_ptr<uint8_t[]> data, uint8_t numMipLevels)
	: width(width)
	, height(height)
	, format(format)
	, numMipLevels(numMipLevels)
	, data(std::move(data))
{
}

Texture::Texture(uint16_t width, uint16_t height, EFormat format, const void* pDataToCopy, uint8_t numMipLevels)
	: width(width)
	, height(height)
	, format(format)
	, numMipLevels(numMipLevels)
	, data(std::make_unique<uint8_t[]>(GetDataSize()))
{
	if(pDataToCopy != nullptr)
	{
		memcpy(data.get(), pDataToCopy, GetDataSize());
	}
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

uint32_t Texture::GetDataSize() const
{
	return static_cast<uint32_t>(width) * height * GetBytesPerPixel();
}

uint8_t Texture::GetBytesPerPixel() const
{
	switch(format)
	{
		case EFormat::R8:			return 1;
		case EFormat::RG8:			return 2;
		case EFormat::RGB8:			return 3;
		case EFormat::RGBA8:		return 4;
		case EFormat::RGB32F:		return 3 * 4;
		case EFormat::RGBA32F:		return 4 * 4;
		case EFormat::D16:			return 2;
		case EFormat::D24_S8:		return 4;
		case EFormat::D32:			return 4;
	}

	return 0;
}
