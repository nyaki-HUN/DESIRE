#include "stdafx.h"
#include "Render/Direct3D11/RenderD3D11.h"
#include "Render/Direct3D11/MeshRenderDataD3D11.h"
#include "Render/Direct3D11/ShaderRenderDataD3D11.h"
#include "Render/Direct3D11/TextureRenderDataD3D11.h"
#include "Render/Material.h"
#include "Core/IWindow.h"
#include "Core/String.h"
#include "Core/fs/FileSystem.h"
#include "Core/fs/IReadFile.h"
#include "Core/math/vectormath.h"
#include "Resource/Mesh.h"
#include "Resource/Shader.h"
#include "Resource/Texture.h"

#include <d3dcompiler.h>

RenderD3D11::RenderD3D11()
	: d3dDevice(nullptr)
	, swapChain(nullptr)
	, renderTargetView(nullptr)
	, deviceCtx(nullptr)
	, initialized(false)
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
		nullptr,
		TRUE,
		DXGI_SWAP_EFFECT_DISCARD,
		0
	};

	DXGI_SWAP_CHAIN_DESC swapChainDescTmp = swapChainDesc;
	swapChainDescTmp.OutputWindow = (HWND)mainWindow->GetHandle();

	HRESULT hr = D3D11CreateDeviceAndSwapChain(
		nullptr,					// might fail with two adapters in machine
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
#if defined(DESIRE_DEBUG)
		D3D11_CREATE_DEVICE_DEBUG,
#else
		D3D11_CREATE_DEVICE_SINGLETHREADED,
#endif
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&swapChainDescTmp,
		&swapChain,
		&d3dDevice,
		nullptr,
		&deviceCtx);

	initialized = SUCCEEDED(hr);

	deviceCtx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// Create a back buffer render target, get a view on it to clear it later
	ID3D11Texture2D *pBackBuffer = nullptr;
	hr = swapChain->GetBuffer(0, IID_ID3D11Texture2D, (void**)&pBackBuffer);
	SUCCEEDED(hr);

	hr = d3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &renderTargetView);
	deviceCtx->OMSetRenderTargets(1, &renderTargetView, nullptr);

	SetViewport(0, 0, mainWindow->GetWidth(), mainWindow->GetHeight());
}

void RenderD3D11::UpdateRenderWindow(IWindow *window)
{
	if(!initialized)
	{
		return;
	}

	swapChain->ResizeBuffers(0, window->GetWidth(), window->GetHeight(), DXGI_FORMAT_UNKNOWN, 0);
}

void RenderD3D11::Kill()
{
	initialized = false;
}

String RenderD3D11::GetShaderFilenameWithPath(const char *shaderFilename) const
{
	return String::CreateFormattedString("data/shaders/hlsl/%s.hlsl", shaderFilename);
}

void RenderD3D11::BeginFrame(IWindow *window)
{
	SetViewport(0, 0, window->GetWidth(), window->GetHeight());

	deviceCtx->ClearRenderTargetView(renderTargetView, clearColor);
}

void RenderD3D11::EndFrame()
{
	swapChain->Present(1, 0);
}

void RenderD3D11::SetViewProjectionMatrices(const Matrix4& viewMatrix, const Matrix4& projMatrix)
{

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

void RenderD3D11::Bind(Mesh *mesh)
{
	ASSERT(mesh != nullptr);

	if(mesh->renderData != nullptr)
	{
		// Already bound
		return;
	}

	MeshRenderDataD3D11 *renderData = new MeshRenderDataD3D11();

	static const D3D11_INPUT_ELEMENT_DESC attribConversionTable[] =
	{
		{ "POSITION",	0,	DXGI_FORMAT_R32G32B32_FLOAT,	0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },		// Mesh::EAttrib::POSITION
		{ "NORMAL",		0,	DXGI_FORMAT_R32G32B32_FLOAT,	0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },		// Mesh::EAttrib::NORMAL
		{ "TEXCOORD",	0,	DXGI_FORMAT_R32G32_FLOAT,		0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },		// Mesh::EAttrib::TEXCOORD0
		{ "TEXCOORD",	1,	DXGI_FORMAT_R32G32_FLOAT,		0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },		// Mesh::EAttrib::TEXCOORD1
		{ "TEXCOORD",	2,	DXGI_FORMAT_R32G32_FLOAT,		0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },		// Mesh::EAttrib::TEXCOORD2
		{ "COLOR",		0,	DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },		// Mesh::EAttrib::COLOR
	};
	DESIRE_CHECK_ARRAY_SIZE(attribConversionTable, Mesh::EAttrib);

	static const DXGI_FORMAT attribTypeConversionTable[][4] =
	{
		// Mesh::EAttribType::FLOAT
		{
			DXGI_FORMAT_R32_FLOAT,
			DXGI_FORMAT_R32G32_FLOAT,
			DXGI_FORMAT_R32G32B32_FLOAT,
			DXGI_FORMAT_R32G32B32A32_FLOAT
		},
		// Mesh::EAttribType::UINT8
		{
			DXGI_FORMAT_R8_UNORM,
			DXGI_FORMAT_R8G8_UNORM,
			DXGI_FORMAT_UNKNOWN,
			DXGI_FORMAT_R8G8B8A8_UNORM,
		},
	};
	DESIRE_CHECK_ARRAY_SIZE(attribTypeConversionTable, Mesh::EAttribType);

	renderData->vertexElementDesc = std::make_unique<D3D11_INPUT_ELEMENT_DESC[]>(mesh->vertexDecl.size());
	uint32_t offset = 0;
	for(Mesh::SVertexDecl& decl : mesh->vertexDecl)
	{
		D3D11_INPUT_ELEMENT_DESC& elementDesc = renderData->vertexElementDesc[renderData->vertexElementDescCount];
		elementDesc = attribConversionTable[(size_t)decl.attrib];
		elementDesc.Format = attribTypeConversionTable[(size_t)decl.type][decl.count - 1];
		elementDesc.AlignedByteOffset = offset;

		renderData->vertexElementDescCount++;
		offset += decl.GetSizeInBytes();
	}

	D3D11_BUFFER_DESC indexBufferDesc = {};
	indexBufferDesc.ByteWidth = mesh->GetSizeOfIndices();
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_BUFFER_DESC vertexBufferDesc = {};
	vertexBufferDesc.ByteWidth = mesh->GetSizeOfVertices();
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	switch(mesh->type)
	{
		case Mesh::EType::STATIC:
			indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
			vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
			indexBufferDesc.CPUAccessFlags = 0;
			break;

		case Mesh::EType::DYNAMIC:
			indexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
			vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
			indexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			break;

		case Mesh::EType::TRANSIENT:
			indexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
			vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
			indexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			break;
	}

	if(mesh->numIndices != 0)
	{
		D3D11_SUBRESOURCE_DATA initialData = {};
		initialData.pSysMem = mesh->indices;

		HRESULT hr = d3dDevice->CreateBuffer(&indexBufferDesc, &initialData, &renderData->indexBuffer);
		ASSERT(SUCCEEDED(hr));
	}

	if(mesh->numVertices != 0)
	{
		D3D11_SUBRESOURCE_DATA initialData = {};
		initialData.pSysMem = mesh->vertices;

		HRESULT hr = d3dDevice->CreateBuffer(&vertexBufferDesc, &initialData, &renderData->vertexBuffer);
		ASSERT(SUCCEEDED(hr));
	}

	mesh->renderData = renderData;
}

void RenderD3D11::Bind(Shader *shader)
{
	ASSERT(shader != nullptr);

	if(shader->renderData != nullptr)
	{
		// Already bound
		return;
	}
	
	ShaderRenderDataD3D11 *renderData = new ShaderRenderDataD3D11();

	ID3DBlob *errorBlob = nullptr;
	HRESULT hr = D3DCompile(shader->data.data, shader->data.size, shader->name.c_str(), nullptr, nullptr, "main", "vs_5_0", 0, 0, &renderData->shaderCode, &errorBlob);
	if(FAILED(hr))
	{
		if(errorBlob != nullptr)
		{
			LOG_ERROR("Shader compile error: %s", (char*)errorBlob->GetBufferPointer());
			errorBlob->Release();
		}
	}
	else
	{
		hr = d3dDevice->CreateVertexShader(renderData->shaderCode->GetBufferPointer(), renderData->shaderCode->GetBufferSize(), nullptr, &renderData->vertexShader);
		ASSERT(SUCCEEDED(hr));
	}

	renderData->ptr->SetPrivateData(WKPDID_D3DDebugObjectName, shader->name.Length(), shader->name.c_str());

	ID3D11ShaderReflection *reflection = nullptr;
	hr = D3DReflect(renderData->shaderCode->GetBufferPointer(), renderData->shaderCode->GetBufferSize() , IID_ID3D11ShaderReflection, (void**)&reflection);
	if(FAILED(hr))
	{
		LOG_ERROR("D3DReflect failed 0x%08x", (uint32_t)hr);
	}

	D3D11_SHADER_DESC shaderDesc;
	hr = reflection->GetDesc(&shaderDesc);
	if(FAILED(hr))
	{
		LOG_ERROR("ID3D11ShaderReflection::GetDesc failed 0x%08x", (uint32_t)hr);
	}

	uint32_t constantBufferSize = 0;
	if(shaderDesc.ConstantBuffers > 0)
	{
		ID3D11ShaderReflectionConstantBuffer *cbuffer = reflection->GetConstantBufferByIndex(0);
		D3D11_SHADER_BUFFER_DESC bufferDesc;
		hr = cbuffer->GetDesc(&bufferDesc);
		ASSERT(SUCCEEDED(hr));

		constantBufferSize = bufferDesc.Size;
	}

	D3D11_BUFFER_DESC desc = {};
	desc.ByteWidth = constantBufferSize;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	hr = d3dDevice->CreateBuffer(&desc, nullptr, &renderData->constantBuffer);
	ASSERT(SUCCEEDED(hr));

	shader->renderData = renderData;
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

void RenderD3D11::Unbind(Mesh *mesh)
{
	if(mesh == nullptr || mesh->renderData == nullptr)
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

void RenderD3D11::Unbind(Shader *shader)
{
	if(shader == nullptr || shader->renderData == nullptr)
	{
		// Not yet bound
		return;
	}

	ShaderRenderDataD3D11 *renderData = static_cast<ShaderRenderDataD3D11*>(shader->renderData);

	delete renderData;
	shader->renderData = nullptr;
}

void RenderD3D11::Unbind(Texture *texture)
{
	if(texture == nullptr || texture->renderData == nullptr)
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

void RenderD3D11::SetMesh(Mesh *mesh)
{
	MeshRenderDataD3D11 *renderData = static_cast<MeshRenderDataD3D11*>(mesh->renderData);

	// Update buffers
	switch(mesh->type)
	{
		case Mesh::EType::STATIC:
			// No update for static mesh
			break;

		case Mesh::EType::DYNAMIC:
			if(mesh->isUpdateRequiredForDynamicMesh)
			{
				D3D11_MAPPED_SUBRESOURCE mappedIndexBuffer;
				if(FAILED(deviceCtx->Map(renderData->indexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedIndexBuffer)))
				{
					LOG_WARNING("Failed to map index buffer");
					return;
				}

				D3D11_MAPPED_SUBRESOURCE mappedVertexBuffer;
				if(FAILED(deviceCtx->Map(renderData->vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedVertexBuffer)))
				{
					LOG_WARNING("Failed to map vertex buffer");

					deviceCtx->Unmap(renderData->indexBuffer, 0);
					return;
				}

				memcpy(mappedIndexBuffer.pData, mesh->indices, mesh->GetSizeOfIndices());
				deviceCtx->Unmap(renderData->indexBuffer, 0);

				memcpy(mappedVertexBuffer.pData, mesh->vertices, mesh->GetSizeOfVertices());
				deviceCtx->Unmap(renderData->vertexBuffer, 0);

				mesh->isUpdateRequiredForDynamicMesh = false;
			}
			break;

		case Mesh::EType::TRANSIENT:
		{
			D3D11_MAPPED_SUBRESOURCE mappedIndexBuffer;
			if(FAILED(deviceCtx->Map(renderData->indexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedIndexBuffer)))
			{
				LOG_WARNING("Failed to map index buffer");
				return;
			}

			D3D11_MAPPED_SUBRESOURCE mappedVertexBuffer;
			if(FAILED(deviceCtx->Map(renderData->vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedVertexBuffer)))
			{
				LOG_WARNING("Failed to map vertex buffer");

				deviceCtx->Unmap(renderData->indexBuffer, 0);
				return;
			}

			memcpy(mappedIndexBuffer.pData, mesh->indices, mesh->GetSizeOfIndices());
			deviceCtx->Unmap(renderData->indexBuffer, 0);

			memcpy(mappedVertexBuffer.pData, mesh->vertices, mesh->GetSizeOfVertices());
			deviceCtx->Unmap(renderData->vertexBuffer, 0);
			break;
		}
	}

	// Set buffers
	deviceCtx->IASetIndexBuffer(renderData->indexBuffer, DXGI_FORMAT_R16_UINT, renderData->indexOffset);
	deviceCtx->IASetVertexBuffers(0, 1, &renderData->vertexBuffer, &mesh->stride, &renderData->vertexOffset);
}

void RenderD3D11::SetShader(Shader *vertexShader, Shader *pixelShader)
{
	if(vertexShader != nullptr)
	{
		const ShaderRenderDataD3D11 *vertexShaderRenderData = static_cast<const ShaderRenderDataD3D11*>(vertexShader->renderData);

		deviceCtx->VSSetShader(vertexShaderRenderData->vertexShader, nullptr, 0);
		deviceCtx->VSSetConstantBuffers(0, 1, &vertexShaderRenderData->constantBuffer);

		const MeshRenderDataD3D11 *meshRenderData = static_cast<const MeshRenderDataD3D11*>(activeMesh->renderData);
		ID3D11InputLayout *vertexLayout = nullptr;
		HRESULT hr = d3dDevice->CreateInputLayout(meshRenderData->vertexElementDesc.get(), meshRenderData->vertexElementDescCount, vertexShaderRenderData->shaderCode->GetBufferPointer(), vertexShaderRenderData->shaderCode->GetBufferSize(), &vertexLayout);
		ASSERT(SUCCEEDED(hr));

		deviceCtx->IASetInputLayout(vertexLayout);
	}
	else
	{
		deviceCtx->VSSetShader(nullptr, nullptr, 0);
	}

	if(pixelShader != nullptr)
	{
		const ShaderRenderDataD3D11 *pixelShaderRenderData = static_cast<const ShaderRenderDataD3D11*>(pixelShader->renderData);

		deviceCtx->PSSetShader(pixelShaderRenderData->pixelShader, nullptr, 0);
		deviceCtx->PSSetConstantBuffers(0, 1, &pixelShaderRenderData->constantBuffer);
	}
	else
	{
		deviceCtx->PSSetShader(nullptr, nullptr, 0);
	}
}

void RenderD3D11::SetTexture(uint8_t samplerIdx, Texture *texture)
{
	ASSERT(samplerIdx < D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT);

	TextureRenderDataD3D11 *renderData = static_cast<TextureRenderDataD3D11*>(texture->renderData);

	deviceCtx->VSSetShaderResources(samplerIdx, 1, &renderData->textureSRV);
	deviceCtx->VSSetSamplers(samplerIdx, 1, &renderData->samplerState);

	deviceCtx->PSSetShaderResources(samplerIdx, 1, &renderData->textureSRV);
	deviceCtx->PSSetSamplers(samplerIdx, 1, &renderData->samplerState);
}

void RenderD3D11::DoRender()
{
	if(activeMesh->numIndices != 0)
	{
		deviceCtx->DrawIndexed(activeMesh->numIndices, 0, 0);
	}
	else
	{
		deviceCtx->Draw(activeMesh->numVertices, 0);
	}
}
