#include "Engine/stdafx.h"
#include "Engine/Resource/Texture.h"

#include "Engine/Render/Render.h"

Texture::Texture(uint16_t width, uint16_t height, EFormat format, std::unique_ptr<uint8_t[]> dataToMove, uint8_t numMipLevels)
	: width(width)
	, height(height)
	, format(format)
	, numMipLevels(numMipLevels)
	, data(std::move(dataToMove))
{
	ASSERT(data != nullptr);
}

Texture::Texture(uint16_t width, uint16_t height, EFormat format, const void* pDataToCopy, uint8_t numMipLevels)
	: width(width)
	, height(height)
	, format(format)
	, numMipLevels(numMipLevels)
{
	if(pDataToCopy != nullptr)
	{
		data = std::make_unique<uint8_t[]>(GetDataSize());
		memcpy(data.get(), pDataToCopy, GetDataSize());
	}
}

Texture::~Texture()
{
	Modules::Render->Unbind(this);
}

bool Texture::IsDepthFormat() const
{
	return format == Texture::EFormat::D16
		|| format == Texture::EFormat::D24_S8
		|| format == Texture::EFormat::D32;
}

uint32_t Texture::GetDataSize() const
{
	return static_cast<uint32_t>(width) * height * GetBytesPerPixel(format);
}
