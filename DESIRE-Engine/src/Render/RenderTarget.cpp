#include "stdafx.h"
#include "Render/RenderTarget.h"
#include "Render/IRender.h"
#include "Resource/Texture.h"

RenderTarget::RenderTarget(uint16_t width, uint16_t height)
{
	textures.emplace_back(std::make_shared<Texture>(width, height, Texture::EFormat::RGBA8));
	textures.emplace_back(std::make_shared<Texture>(width, height, Texture::EFormat::D24S8));
}

RenderTarget::~RenderTarget()
{
	IRender::Get()->Unbind(this);
}

uint16_t RenderTarget::GetWidth() const
{
	return textures[0]->width;
}

uint16_t RenderTarget::GetHeight() const
{
	return textures[0]->height;
}

uint8_t RenderTarget::GetTextureCount() const
{
	return (uint8_t)textures.size();
}

const std::shared_ptr<Texture>& RenderTarget::GetTexture(uint8_t idx) const
{
	ASSERT(idx < textures.size());
	return textures[idx];
}
