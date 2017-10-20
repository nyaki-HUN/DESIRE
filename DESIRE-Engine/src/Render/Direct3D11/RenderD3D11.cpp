#include "stdafx.h"
#include "Render/Direct3D11/RenderD3D11.h"
#include "Render/Direct3D11/MeshRenderDataD3D11.h"
#include "Core/IWindow.h"
#include "Core/String.h"
#include "Core/fs/FileSystem.h"
#include "Core/fs/IReadFile.h"
#include "Resource/Mesh.h"
#include "Resource/Texture.h"

RenderD3D11::RenderD3D11()
	: initialized(false)
{
	memset(clearColor, 0, sizeof(clearColor));
}

RenderD3D11::~RenderD3D11()
{

}

void RenderD3D11::Init(IWindow *mainWindow)
{
	static const DXGI_SWAP_CHAIN_DESC swapChainDesc =
	{
		{ mainWindow->GetWidth(), mainWindow->GetHeight(), { 60, 1 }, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED, DXGI_MODE_SCALING_UNSPECIFIED },
		{ 1, 0 },
		DXGI_USAGE_RENDER_TARGET_OUTPUT,
		1,
		NULL,
		TRUE,
		DXGI_SWAP_EFFECT_DISCARD,
		0
	};

	DXGI_SWAP_CHAIN_DESC swapChainDescTmp = swapChainDesc;
	swapChainDescTmp.OutputWindow = (HWND)mainWindow->GetHandle();

	HRESULT hr = D3D11CreateDeviceAndSwapChain(
		NULL,					// might fail with two adapters in machine
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
#if defined(_DEBUG)
		D3D11_CREATE_DEVICE_DEBUG,
#else
		D3D11_CREATE_DEVICE_SINGLETHREADED,
#endif
		NULL,
		0,
		D3D11_SDK_VERSION,
		&swapChainDescTmp,
		&swapChain,
		&d3dDevice,
		NULL,
		&immediateContext);

	initialized = SUCCEEDED(hr);

	immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// Create a back buffer render target, get a view on it to clear it later
	ID3D11Texture2D *pBackBuffer = nullptr;
	hr = swapChain->GetBuffer(0, IID_ID3D11Texture2D, (void**)&pBackBuffer);

	hr = d3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &renderTargetView);
	immediateContext->OMSetRenderTargets(1, &renderTargetView, NULL);

	UpdateRenderWindow(mainWindow);
}

void RenderD3D11::Kill()
{
	initialized = false;
}

void RenderD3D11::UpdateRenderWindow(IWindow *window)
{
	if(!initialized)
	{
		return;
	}

	const D3D11_VIEWPORT vp = { 0.0f, 0.0f, (float)window->GetWidth(), (float)window->GetHeight(), 0, 1 };
	immediateContext->RSSetViewports(1, &vp);
}

void RenderD3D11::BeginFrame(IWindow *window)
{
	immediateContext->ClearRenderTargetView(renderTargetView, clearColor);
}

void RenderD3D11::EndFrame()
{
	swapChain->Present(1, 0);
}

void RenderD3D11::Bind(Mesh *mesh)
{
	ASSERT(mesh != nullptr);

	if(mesh->renderData != nullptr)
	{
		// Already bound
		return;
	}

	MeshRenderDataD3D11 *renderData = new MeshRenderDataD3D11();

	HRESULT hr;
//	hr = d3dDevice->CreateBuffer(&bufferDesc, &initData, &renderData->indexBuffer);
//	hr = d3dDevice->CreateBuffer(&bufferDesc, &initData, &renderData->vertexBuffer);

	mesh->renderData = renderData;
}

void RenderD3D11::UnBind(Mesh *mesh)
{
	ASSERT(mesh != nullptr);

	if(mesh->renderData == nullptr)
	{
		// Not yet bound
		return;
	}

	MeshRenderDataD3D11 *renderData = static_cast<MeshRenderDataD3D11*>(mesh->renderData);
	if(renderData->indexBuffer != nullptr)
	{
		renderData->indexBuffer->Release();
	}
	if(renderData->vertexBuffer != nullptr)
	{
		renderData->vertexBuffer->Release();
	}

	delete renderData;
	mesh->renderData = nullptr;
}

void RenderD3D11::Bind(Texture *texture)
{
	ASSERT(texture != nullptr);

	if(texture->renderData != nullptr)
	{
		// Already bound
		return;
	}

//	texture->renderData = ;
}

void RenderD3D11::Unbind(Texture *texture)
{
	ASSERT(texture != nullptr);

	if(texture->renderData == nullptr)
	{
		// Not yet bound
		return;
	}

//	delete renderData;
	texture->renderData = nullptr;
}
