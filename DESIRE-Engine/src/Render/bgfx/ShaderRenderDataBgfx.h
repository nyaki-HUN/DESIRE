#pragma once

#include "bgfx/bgfx.h"

class ShaderRenderDataBgfx
{
public:
	bgfx::ShaderHandle shaderHandle = BGFX_INVALID_HANDLE;
	bgfx::UniformHandle u_tint = BGFX_INVALID_HANDLE;

	bgfx::ProgramHandle shaderProgram = BGFX_INVALID_HANDLE;
};
