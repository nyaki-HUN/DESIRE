#pragma once

#include "Engine/Render/RenderData.h"

class RenderableRenderDataBgfx : public RenderData
{
public:
	~RenderableRenderDataBgfx() override
	{
		bgfx::destroy(m_shaderProgram);
	}

	bgfx::ProgramHandle m_shaderProgram;
	uint64_t m_renderState = 0;
};
