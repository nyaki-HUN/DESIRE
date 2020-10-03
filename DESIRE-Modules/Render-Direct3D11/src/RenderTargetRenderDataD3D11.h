#pragma once

class RenderTargetRenderDataD3D11
{
public:
	std::vector<ID3D11RenderTargetView*> m_RTVs;
	ID3D11DepthStencilView* m_pDSV = nullptr;
};
