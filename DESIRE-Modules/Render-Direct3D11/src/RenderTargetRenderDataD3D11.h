#pragma once

class RenderTargetRenderDataD3D11
{
public:
	std::vector<ID3D11RenderTargetView*> m_renderTargetViews;
	ID3D11DepthStencilView* m_pDepthStencilView = nullptr;
};
