#include "stdafx.h"
#include "Render/Direct3D11/RenderD3D11.h"
#include "Render/Direct3D11/MeshRenderDataD3D11.h"
#include "Render/Direct3D11/TextureRenderDataD3D11.h"
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
		&deviceCtx);

	initialized = SUCCEEDED(hr);

	deviceCtx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// Create a back buffer render target, get a view on it to clear it later
	ID3D11Texture2D *pBackBuffer = nullptr;
	hr = swapChain->GetBuffer(0, IID_ID3D11Texture2D, (void**)&pBackBuffer);

	hr = d3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &renderTargetView);
	deviceCtx->OMSetRenderTargets(1, &renderTargetView, NULL);

	SetViewport(0, 0, mainWindow->GetWidth(), mainWindow->GetHeight());
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

	swapChain->ResizeBuffers(0, window->GetWidth(), window->GetHeight(), DXGI_FORMAT_UNKNOWN, 0);

	SetViewport(0, 0, window->GetWidth(), window->GetHeight());
}

void RenderD3D11::BeginFrame(IWindow *window)
{
	DESIRE_UNUSED(window);

	deviceCtx->ClearRenderTargetView(renderTargetView, clearColor);
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

	D3D11_BUFFER_DESC indexBufferDesc = {};
	indexBufferDesc.ByteWidth = mesh->GetSizeOfIndices();
	indexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	HRESULT hr = d3dDevice->CreateBuffer(&indexBufferDesc, NULL, &renderData->indexBuffer);

	D3D11_BUFFER_DESC vertexBufferDesc = {};
	vertexBufferDesc.ByteWidth = mesh->GetSizeOfVertices();
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	hr = d3dDevice->CreateBuffer(&vertexBufferDesc, NULL, &renderData->vertexBuffer);

	mesh->renderData = renderData;
}

void RenderD3D11::Unbind(Mesh *mesh)
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

void RenderD3D11::SetMesh(Mesh *mesh)
{
	MeshRenderDataD3D11 *renderData = static_cast<MeshRenderDataD3D11*>(mesh->renderData);

	switch(mesh->type)
	{
		case Mesh::EType::STATIC:
			break;

		case Mesh::EType::DYNAMIC:
			if(mesh->isUpdateRequiredForDynamicMesh)
			{
				mesh->isUpdateRequiredForDynamicMesh = false;
			}
			break;

		case Mesh::EType::TRANSIENT:
		{
			D3D11_MAPPED_SUBRESOURCE mappedIndexBuffer;
			if(FAILED(deviceCtx->Map(renderData->indexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedIndexBuffer)))
			{
				return;
			}

			D3D11_MAPPED_SUBRESOURCE mappedVertexBuffer;
			if(FAILED(deviceCtx->Map(renderData->vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedVertexBuffer)))
			{
				return;
			}

			memcpy(mappedIndexBuffer.pData, mesh->indices, mesh->GetSizeOfIndices());
			memcpy(mappedVertexBuffer.pData, mesh->vertices, mesh->GetSizeOfVertices());

			deviceCtx->Unmap(renderData->indexBuffer, 0);
			deviceCtx->Unmap(renderData->vertexBuffer, 0);
			break;
		}
	}
}

void RenderD3D11::Bind(Texture *texture)
{
	ASSERT(texture != nullptr);

	if(texture->renderData != nullptr)
	{
		// Already bound
		return;
	}

	TextureRenderDataD3D11 *renderData = new TextureRenderDataD3D11();

	uint8_t bitsPerPixel = 0;
	DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;
	switch(texture->format)
	{
		case Texture::EFormat::UNKNOWN:
			bitsPerPixel = 0;
			format = DXGI_FORMAT_UNKNOWN;
			break;

		case Texture::EFormat::R8:
			bitsPerPixel = 8;
			format = DXGI_FORMAT_R8_UNORM;
			break;

		case Texture::EFormat::RG8:
			bitsPerPixel = 16;
			format = DXGI_FORMAT_R8G8_UNORM;
			break;

		case Texture::EFormat::RGB8:
			bitsPerPixel = 24;
			format = DXGI_FORMAT_UNKNOWN;
			break;

		case Texture::EFormat::RGBA8:
			bitsPerPixel = 32;
			format = DXGI_FORMAT_R8G8B8A8_UNORM;
			break;

		case Texture::EFormat::RGBA32F:
			bitsPerPixel = 128;
			format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			break;
	}

	D3D11_SUBRESOURCE_DATA subResourceData;
	subResourceData.pSysMem = texture->data.data;
	subResourceData.SysMemPitch = texture->width * bitsPerPixel / 8;
	subResourceData.SysMemSlicePitch = 0;

	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = texture->width;
	desc.Height = texture->height;
	desc.MipLevels = texture->numMipMaps + 1;
	desc.ArraySize = 1;
	desc.Format = format;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
//	desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

	ID3D11Texture2D *d3dTexture2D = nullptr;
	d3dDevice->CreateTexture2D(&desc, &subResourceData, &d3dTexture2D);

	// Create texture view
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = desc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;
	d3dDevice->CreateShaderResourceView(d3dTexture2D, &srvDesc, &renderData->textureSRV);
	
	d3dTexture2D->Release();

	// Create texture sampler
	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	d3dDevice->CreateSamplerState(&samplerDesc, &renderData->samplerState);

	texture->renderData = renderData;
}

void RenderD3D11::Unbind(Texture *texture)
{
	ASSERT(texture != nullptr);

	if(texture->renderData == nullptr)
	{
		// Not yet bound
		return;
	}

	TextureRenderDataD3D11 *renderData = static_cast<TextureRenderDataD3D11*>(texture->renderData);
	renderData->textureSRV->Release();
	renderData->samplerState->Release();

	delete renderData;
	texture->renderData = nullptr;
}

void RenderD3D11::SetTexture(uint8_t samplerIdx, Texture *texture)
{
	if(texture->renderData == nullptr)
	{
		Bind(texture);
	}

	TextureRenderDataD3D11 *renderData = static_cast<TextureRenderDataD3D11*>(texture->renderData);

	deviceCtx->VSSetShaderResources(samplerIdx, 1, &renderData->textureSRV);
	deviceCtx->VSSetSamplers(samplerIdx, 1, &renderData->samplerState);

	deviceCtx->PSSetShaderResources(samplerIdx, 1, &renderData->textureSRV);
	deviceCtx->PSSetSamplers(samplerIdx, 1, &renderData->samplerState);
}

void RenderD3D11::SetViewport(uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
	const D3D11_VIEWPORT vp = { (float)x, (float)y, (float)width, (float)height, 0.0f, 1.0f };
	deviceCtx->RSSetViewports(1, &vp);
}

void RenderD3D11::SetScissor(uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
	const D3D11_RECT rect = { x, y, x + width, y + height };
	deviceCtx->RSSetScissorRects(1, &rect);
}
