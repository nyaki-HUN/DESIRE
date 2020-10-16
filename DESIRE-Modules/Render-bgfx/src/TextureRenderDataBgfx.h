#pragma once

#include "Engine/Render/RenderData.h"

class TextureRenderDataBgfx : public RenderData
{
public:
	~TextureRenderDataBgfx() override
	{
		bgfx::destroy(m_textureHandle);
	}

	bgfx::TextureHandle m_textureHandle = BGFX_INVALID_HANDLE;
};
