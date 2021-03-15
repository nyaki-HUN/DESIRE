#pragma once

#include "Engine/Render/RenderData.h"

class VertexBufferRenderDataBgfx : public RenderData
{
public:
	~VertexBufferRenderDataBgfx() override
	{
		bgfx::destroy(m_vertexBuffer);
		bgfx::destroy(m_dynamicVertexBuffer);
	}

	bgfx::VertexBufferHandle m_vertexBuffer = BGFX_INVALID_HANDLE;
	bgfx::DynamicVertexBufferHandle m_dynamicVertexBuffer = BGFX_INVALID_HANDLE;

	bgfx::VertexLayout m_vertexLayout;
};
