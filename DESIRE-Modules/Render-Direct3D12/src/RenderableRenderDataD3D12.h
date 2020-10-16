#pragma once

#include "Engine/Render/RenderData.h"

class RenderableRenderDataD3D12 : public RenderData
{
public:
	~RenderableRenderDataD3D12() override
	{
		DX_SAFE_RELEASE(m_pPipelineState);
	}

	ID3D12PipelineState* m_pPipelineState = nullptr;
};
