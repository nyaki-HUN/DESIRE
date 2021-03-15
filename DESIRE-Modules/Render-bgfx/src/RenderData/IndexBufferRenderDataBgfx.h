#pragma once

#include "Engine/Render/RenderData.h"

class IndexBufferRenderDataBgfx : public RenderData
{
public:
	~IndexBufferRenderDataBgfx() override
	{
		bgfx::destroy(m_indexBuffer);
		bgfx::destroy(m_dynamicIndexBuffer);
	}

	bgfx::IndexBufferHandle m_indexBuffer = BGFX_INVALID_HANDLE;
	bgfx::DynamicIndexBufferHandle m_dynamicIndexBuffer = BGFX_INVALID_HANDLE;
};
