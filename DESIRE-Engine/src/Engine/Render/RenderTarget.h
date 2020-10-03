#pragma once

#include "Engine/Core/Container/Array.h"

class Texture;

class RenderTarget
{
public:
	RenderTarget(uint16_t width, uint16_t height);
	~RenderTarget();

	uint16_t GetWidth() const;
	uint16_t GetHeight() const;

	uint8_t GetTextureCount() const;
	const std::shared_ptr<Texture>& GetTexture(uint8_t idx = 0) const;

	// Render engine specific data set at bind
	void* m_pRenderData = nullptr;

private:
	Array<std::shared_ptr<Texture>> m_textures;
};
