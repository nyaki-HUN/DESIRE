#pragma once

#include "bgfx/bgfx.h"

class ShaderRenderDataBgfx
{
public:
	bgfx::ShaderHandle shaderHandle;
	bgfx::ProgramHandle shaderProgram;
	bgfx::UniformHandle u_tint;
};
