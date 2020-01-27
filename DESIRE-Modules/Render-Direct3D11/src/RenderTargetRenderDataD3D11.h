#pragma once

class RenderTargetRenderDataD3D11
{
public:
	std::vector<ID3D11RenderTargetView*> renderTargetViews;
	ID3D11DepthStencilView* depthStencilView = nullptr;
};
