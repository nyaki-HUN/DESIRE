#pragma once

class RenderTargetRenderDataD3D11
{
public:
	ID3D11RenderTargetView* m_RTVs[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT] = {};
	uint32_t m_numRTVs = 0;

	ID3D11DepthStencilView* m_pDSV = nullptr;
};
