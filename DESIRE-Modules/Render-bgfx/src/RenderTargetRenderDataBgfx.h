#pragma once

#include "bgfx/bgfx.h"

class RenderTargetRenderDataBgfx
{
public:
	bgfx::FrameBufferHandle frameBuffer = BGFX_INVALID_HANDLE;
	uint8_t id = 1;
};
