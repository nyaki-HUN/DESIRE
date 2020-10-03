#include "Engine/stdafx.h"
#include "Engine/Render/RenderTarget.h"

#include "Engine/Render/Render.h"
#include "Engine/Render/Texture.h"

RenderTarget::RenderTarget(uint16_t width, uint16_t height)
{
	m_textures.EmplaceAdd(std::make_shared<Texture>(width, height, Texture::EFormat::RGBA8));
	m_textures.EmplaceAdd(std::make_shared<Texture>(width, height, Texture::EFormat::D24_S8));
}

RenderTarget::~RenderTarget()
{
	Modules::Render->Unbind(*this);
}

uint16_t RenderTarget::GetWidth() const
{
	return m_textures[0]->GetWidth();
}

uint16_t RenderTarget::GetHeight() const
{
	return m_textures[0]->GetHeight();
}

uint8_t RenderTarget::GetTextureCount() const
{
	return static_cast<uint8_t>(m_textures.Size());
}

const std::shared_ptr<Texture>& RenderTarget::GetTexture(uint8_t idx) const
{
	return m_textures[idx];
}
