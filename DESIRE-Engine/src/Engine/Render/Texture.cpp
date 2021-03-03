#include "Engine/stdafx.h"
#include "Engine/Render/Texture.h"

#include "Engine/Render/Render.h"

Texture::Texture(uint16_t width, uint16_t height, EFormat format, std::unique_ptr<uint8_t[]> spDataToMove, uint8_t numMipLevels)
	: m_width(width)
	, m_height(height)
	, m_format(format)
	, m_numMipLevels(numMipLevels)
	, m_spData(std::move(spDataToMove))
{
	ASSERT(m_spData != nullptr);
}

Texture::Texture(uint16_t width, uint16_t height, EFormat format, const void* pDataToCopy, uint8_t numMipLevels)
	: m_width(width)
	, m_height(height)
	, m_format(format)
	, m_numMipLevels(numMipLevels)
{
	if(pDataToCopy)
	{
		m_spData = std::make_unique<uint8_t[]>(GetDataSize());
		memcpy(m_spData.get(), pDataToCopy, GetDataSize());
	}
}

Texture::~Texture()
{
	Modules::Render->Unbind(*this);
}

Texture& Texture::operator =(Texture&& otherTexture)
{
	Modules::Render->Unbind(*this);

	m_pRenderData = otherTexture.m_pRenderData;
	m_width = otherTexture.m_width;
	m_height = otherTexture.m_height;
	m_format = otherTexture.m_format;
	m_numMipLevels = otherTexture.m_numMipLevels;
	m_spData = std::move(otherTexture.m_spData);

	otherTexture.m_pRenderData = nullptr;

	return *this;
}

uint16_t Texture::GetWidth() const
{
	return m_width;
}

uint16_t Texture::GetHeight() const
{
	return m_height;
}

Texture::EFormat Texture::GetFormat() const
{
	return m_format;
}

uint8_t Texture::GetNumMipLevels() const
{
	return m_numMipLevels;
}

const uint8_t* Texture::GetData() const
{
	return m_spData.get();
}

uint32_t Texture::GetDataSize() const
{
	return static_cast<uint32_t>(m_width) * m_height * GetBytesPerPixel();
}

bool Texture::IsDepthFormat() const
{
	return m_format == Texture::EFormat::D16
		|| m_format == Texture::EFormat::D24_S8
		|| m_format == Texture::EFormat::D32;
}

uint8_t Texture::GetBytesPerPixel() const
{
	return GetBytesPerPixelForFormat(m_format);
}
