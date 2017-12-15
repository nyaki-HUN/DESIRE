#pragma once

#include <d3d11.h>

class RenderTargetRenderDataD3D11
{
public:
	std::vector<ID3D11RenderTargetView*> renderTargetViews;
	ID3D11DepthStencilView *depthStencilView = nullptr;
};
