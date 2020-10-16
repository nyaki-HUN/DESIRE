#pragma once

#include "Engine/Render/RenderData.h"

class RenderTargetRenderDataD3D11 : public RenderData
{
public:
	~RenderTargetRenderDataD3D11() override
	{
		for(uint32_t i = 0; i < m_numRTVs; ++i)
		{
			DX_SAFE_RELEASE(m_RTVs[i]);
		}

		DX_SAFE_RELEASE(m_pDSV);
	}

	ID3D11RenderTargetView* m_RTVs[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT] = {};
	uint32_t m_numRTVs = 0;

	ID3D11DepthStencilView* m_pDSV = nullptr;
};
