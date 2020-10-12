#pragma once

class RenderableRenderDataBgfx
{
public:
	bgfx::ProgramHandle m_shaderProgram;
	uint64_t m_renderState = 0;
};
