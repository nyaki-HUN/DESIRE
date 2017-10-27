#pragma once

#include "bgfx/bgfx.h"

class ShaderRenderDataBgfx
{
public:
	bgfx::ProgramHandle shaderProgram;
	bgfx::UniformHandle u_tint;
};
