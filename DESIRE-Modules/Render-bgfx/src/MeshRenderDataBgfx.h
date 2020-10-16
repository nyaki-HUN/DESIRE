#pragma once

#include "Engine/Render/RenderData.h"

class MeshRenderDataBgfx : public RenderData
{
public:
	~MeshRenderDataBgfx() override
	{
		bgfx::destroy(m_indexBuffer);
		bgfx::destroy(m_vertexBuffer);

		bgfx::destroy(m_dynamicIndexBuffer);
		bgfx::destroy(m_dynamicVertexBuffer);
	}

	bgfx::IndexBufferHandle m_indexBuffer = BGFX_INVALID_HANDLE;
	bgfx::VertexBufferHandle m_vertexBuffer = BGFX_INVALID_HANDLE;

	bgfx::DynamicIndexBufferHandle m_dynamicIndexBuffer = BGFX_INVALID_HANDLE;
	bgfx::DynamicVertexBufferHandle m_dynamicVertexBuffer = BGFX_INVALID_HANDLE;

	bgfx::VertexLayout m_vertexLayout;
};
