#pragma once

#include "Engine/Core/HashedStringMap.h"

#include "Engine/Render/RenderData.h"

class ShaderRenderDataBgfx : public RenderData
{
public:
	~ShaderRenderDataBgfx() override
	{
		bgfx::destroy(m_shaderHandle);
	}

	bgfx::ShaderHandle m_shaderHandle = BGFX_INVALID_HANDLE;
	HashedStringMap<bgfx::UniformHandle> m_uniforms;
};
