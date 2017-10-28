#pragma once

#include "bgfx/bgfx.h"

class ShaderRenderDataBgfx
{
public:
	bgfx::ShaderHandle shaderHandle;
	bgfx::UniformHandle u_tint = BGFX_INVALID_HANDLE;

	bgfx::ProgramHandle shaderProgram = BGFX_INVALID_HANDLE;
};
