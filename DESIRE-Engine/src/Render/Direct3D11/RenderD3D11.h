#pragma once

#include "Render/IRender.h"

#include <D3D11.h>

class RenderD3D11 : public IRender
{
public:
	RenderD3D11();
	~RenderD3D11();

	void Init(IWindow *mainWindow) override;
	void Kill() override;

	void UpdateRenderWindow(IWindow *window) override;

	void BeginFrame(IWindow *window) override;
	void EndFrame() override;

	void Bind(Mesh *mesh) override;
	void UnBind(Mesh *mesh) override;

	void Bind(Texture *texture) override;
	void Unbind(Texture *texture) override;

private:
	ID3D11Device *d3dDevice;
	IDXGISwapChain *swapChain;
	ID3D11RenderTargetView *renderTargetView;
	ID3D11DeviceContext *immediateContext;

	float clearColor[4];

	bool initialized;
};
