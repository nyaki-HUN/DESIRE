#pragma once

#include "Engine/Render/RenderData.h"

class RenderTargetRenderDataBgfx : public RenderData
{
public:
	~RenderTargetRenderDataBgfx() override
	{
		bgfx::destroy(m_frameBuffer);
	}

	bgfx::FrameBufferHandle m_frameBuffer = BGFX_INVALID_HANDLE;
	uint8_t m_id = 1;
};
