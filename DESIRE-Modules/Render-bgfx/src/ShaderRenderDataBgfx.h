#pragma once

#include "Engine/Core/HashedStringMap.h"

class ShaderRenderDataBgfx
{
public:
	bgfx::ShaderHandle shaderHandle = BGFX_INVALID_HANDLE;
	HashedStringMap<bgfx::UniformHandle> uniforms;
};
