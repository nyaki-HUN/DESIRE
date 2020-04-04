#include "Engine/stdafx.h"
#include "Engine/Render/RenderTarget.h"

#include "Engine/Render/Render.h"

#include "Engine/Resource/Texture.h"

RenderTarget::RenderTarget(uint16_t width, uint16_t height)
{
	textures.EmplaceAdd(std::make_shared<Texture>(width, height, Texture::EFormat::RGBA8));
	textures.EmplaceAdd(std::make_shared<Texture>(width, height, Texture::EFormat::D24_S8));
}

RenderTarget::~RenderTarget()
{
	if(renderData != nullptr)
	{
		Modules::Render->Unbind(this);
	}
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
	return (uint8_t)textures.Size();
}

const std::shared_ptr<Texture>& RenderTarget::GetTexture(uint8_t idx) const
{
	ASSERT(idx < textures.Size());
	return textures[idx];
}
