#pragma once

#include "Core/HashedStringMap.h"

#include "bgfx/bgfx.h"

class ShaderRenderDataBgfx
{
public:
	bgfx::ShaderHandle shaderHandle = BGFX_INVALID_HANDLE;
	HashedStringMap<bgfx::UniformHandle> uniforms;
};
