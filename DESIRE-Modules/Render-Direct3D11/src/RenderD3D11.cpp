#include "stdafx.h"
#include "RenderD3D11.h"
#include "DirectXMathExt.h"
#include "MeshRenderDataD3D11.h"
#include "ShaderRenderDataD3D11.h"
#include "TextureRenderDataD3D11.h"
#include "RenderTargetRenderDataD3D11.h"

#include "Core/IWindow.h"
#include "Core/String.h"
#include "Core/fs/FileSystem.h"
#include "Core/fs/IReadFile.h"
#include "Core/math/vectormath.h"
#include "Render/Material.h"
#include "Render/RenderTarget.h"
#include "Render/View.h"
#include "Resource/Mesh.h"
#include "Resource/Shader.h"

#include <d3dcompiler.h>

#define DX_RELEASE(ptr)		\
	if(ptr != nullptr)		\
	{						\
		ptr->Release();		\
		ptr = nullptr;		\
	}

RenderD3D11::RenderD3D11()
{
	const char vs_error[] =
	{
		"cbuffer CB : register(b0)\n"
		"{\n"
		"	float4x4 matWorldViewProj;\n"
		"};\n"
		"float4 main(float3 pos : POSITION) : SV_POSITION\n"
		"{\n"
		"	float4 position = mul(matWorldViewProj, float4(pos, 1.0));\n"
		"	return position;\n"
		"}"
	};

	const char ps_error[] =
	{
		"float3 main() : SV_TARGET\n"
		"{\n"
		"	return float3(1, 0, 1);\n"
		"}"
	};

	errorVertexShader = std::make_unique<Shader>("vs_error");
	errorVertexShader->data = MemoryBuffer::CreateFromDataCopy(vs_error, sizeof(vs_error));
	errorPixelShader = std::make_unique<Shader>("ps_error");
	errorPixelShader->data = MemoryBuffer::CreateFromDataCopy(ps_error, sizeof(ps_error));

	matWorld = DirectX::XMMatrixIdentity();
	matView = DirectX::XMMatrixIdentity();
	matProj = DirectX::XMMatrixIdentity();
}

RenderD3D11::~RenderD3D11()
{
	errorVertexShader = nullptr;
	errorPixelShader = nullptr;
}

void RenderD3D11::Init(IWindow *mainWindow)
{
	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChainDesc.BufferDesc.Width = mainWindow->GetWidth();
	swapChainDesc.BufferDesc.Height = mainWindow->GetHeight();
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 1;
	swapChainDesc.OutputWindow = (HWND)mainWindow->GetHandle();
	swapChainDesc.Windowed = TRUE;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

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
		&swapChainDesc,
		&swapChain,
		&d3dDevice,
		nullptr,
		&deviceCtx);

	initialized = SUCCEEDED(hr);

	deviceCtx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Create back buffer render target view
	ID3D11Texture2D *backBufferTexture = nullptr;
	hr = swapChain->GetBuffer(0, IID_ID3D11Texture2D, (void**)&backBufferTexture);
	ASSERT(SUCCEEDED(hr));
	hr = d3dDevice->CreateRenderTargetView(backBufferTexture, nullptr, &backBufferRenderTargetView);
	ASSERT(SUCCEEDED(hr));
	DX_RELEASE(backBufferTexture);

	// Create back buffer depth stencil view
	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = swapChainDesc.BufferDesc.Width;
	textureDesc.Height = swapChainDesc.BufferDesc.Height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	textureDesc.SampleDesc = swapChainDesc.SampleDesc;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	ID3D11Texture2D *depthStencilTexture = nullptr;
	hr = d3dDevice->CreateTexture2D(&textureDesc, nullptr, &depthStencilTexture);
	ASSERT(SUCCEEDED(hr));
	
	hr = d3dDevice->CreateDepthStencilView(depthStencilTexture, nullptr, &backBufferDepthStencilView);
	ASSERT(SUCCEEDED(hr));
	
	DX_RELEASE(depthStencilTexture);

	deviceCtx->OMSetRenderTargets(1, &backBufferRenderTargetView, backBufferDepthStencilView);

	Bind(errorVertexShader.get());
	Bind(errorPixelShader.get());

	// TODO: proper render state handling

	D3D11_RASTERIZER_DESC desc = {};
	desc.FillMode = D3D11_FILL_SOLID;
	desc.CullMode = D3D11_CULL_NONE;
	desc.FrontCounterClockwise = false;
	desc.DepthClipEnable = true;
	desc.ScissorEnable = true;
	desc.MultisampleEnable = true;
	desc.AntialiasedLineEnable = false;
	ID3D11RasterizerState *rs = nullptr;
	hr = d3dDevice->CreateRasterizerState(&desc, &rs);
	deviceCtx->RSSetState(rs);

	D3D11_BLEND_DESC blendDesc = {};
	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.IndependentBlendEnable = false;
	blendDesc.RenderTarget[0].BlendEnable = true;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	ID3D11BlendState *blendState = nullptr;
	d3dDevice->CreateBlendState(&blendDesc, &blendState);
	deviceCtx->OMSetBlendState(blendState, blendFactor, 0xffffffff);

	D3D11_DEPTH_STENCIL_DESC dsDesc = {};
	// Depth test parameters
	dsDesc.DepthEnable = true;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	// Stencil test parameters
	dsDesc.StencilEnable = false;
	dsDesc.StencilReadMask = 0xFF;
	dsDesc.StencilWriteMask = 0xFF;
	// Stencil operations if pixel is front-facing
	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	// Stencil operations if pixel is back-facing
	dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	ID3D11DepthStencilState *depthStencilState = nullptr;
	d3dDevice->CreateDepthStencilState(&dsDesc, &depthStencilState);
	deviceCtx->OMSetDepthStencilState(depthStencilState, 0);
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

	Unbind(errorVertexShader.get());
	Unbind(errorPixelShader.get());

	DX_RELEASE(backBufferDepthStencilView);
	DX_RELEASE(backBufferRenderTargetView);
}

String RenderD3D11::GetShaderFilenameWithPath(const char *shaderFilename) const
{
	return String::CreateFormattedString("data/shaders/hlsl/%s.hlsl", shaderFilename);
}

void RenderD3D11::BeginFrame(IWindow *window)
{
	activeWindow = window;
	SetViewport(0, 0, window->GetWidth(), window->GetHeight());

	deviceCtx->ClearRenderTargetView(backBufferRenderTargetView, clearColor);
	deviceCtx->ClearDepthStencilView(backBufferDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void RenderD3D11::EndFrame()
{
	swapChain->Present(1, 0);
}

void RenderD3D11::SetView(View *view)
{
	if(view != nullptr)
	{
		RenderTarget *rt = view->GetRenderTarget();
		if(rt->renderData == nullptr)
		{
			Bind(rt);
		}

		RenderTargetRenderDataD3D11 *renderData = static_cast<RenderTargetRenderDataD3D11*>(rt->renderData);
		deviceCtx->OMSetRenderTargets((UINT)renderData->renderTargetViews.size(), renderData->renderTargetViews.data(), renderData->depthStencilView);
		SetViewport(view->GetPosX(), view->GetPosY(), view->GetWidth(), view->GetHeight());

		for(ID3D11RenderTargetView *renderTargetView : renderData->renderTargetViews)
		{
			deviceCtx->ClearRenderTargetView(renderTargetView, clearColor);
		}
		if(renderData->depthStencilView != nullptr)
		{
			deviceCtx->ClearDepthStencilView(renderData->depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		}
	}
	else
	{
		deviceCtx->OMSetRenderTargets(1, &backBufferRenderTargetView, backBufferDepthStencilView);
		SetViewport(0, 0, activeWindow->GetWidth(), activeWindow->GetHeight());
	}
}

void RenderD3D11::SetWorldMatrix(const Matrix4& matrix)
{
	matWorld.r[0] = GetXMVECTOR(matrix.col0);
	matWorld.r[1] = GetXMVECTOR(matrix.col1);
	matWorld.r[2] = GetXMVECTOR(matrix.col2);
	matWorld.r[3] = GetXMVECTOR(matrix.col3);
}

void RenderD3D11::SetViewProjectionMatrices(const Matrix4& viewMatrix, const Matrix4& projMatrix)
{
	matView.r[0] = GetXMVECTOR(viewMatrix.col0);
	matView.r[1] = GetXMVECTOR(viewMatrix.col1);
	matView.r[2] = GetXMVECTOR(viewMatrix.col2);
	matView.r[3] = GetXMVECTOR(viewMatrix.col3);

	matProj.r[0] = GetXMVECTOR(projMatrix.col0);
	matProj.r[1] = GetXMVECTOR(projMatrix.col1);
	matProj.r[2] = GetXMVECTOR(projMatrix.col2);
	matProj.r[3] = GetXMVECTOR(projMatrix.col3);
}

void RenderD3D11::SetScissor(uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
	const D3D11_RECT rect = { x, y, x + width, y + height };
	deviceCtx->RSSetScissorRects(1, &rect);
}

void RenderD3D11::SetClearColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	clearColor[0] = r / 255.0f;
	clearColor[1] = g / 255.0f;
	clearColor[2] = b / 255.0f;
	clearColor[3] = a / 255.0f;
}

void RenderD3D11::SetColorWriteEnabled(bool rgbWriteEnabled, bool alphaWriteEnabled)
{
	D3D11_BLEND_DESC blendDesc = {};

	if(rgbWriteEnabled)
	{
		blendDesc.RenderTarget[0].RenderTargetWriteMask |= D3D11_COLOR_WRITE_ENABLE_RED | D3D11_COLOR_WRITE_ENABLE_GREEN | D3D11_COLOR_WRITE_ENABLE_BLUE;
	}

	if(alphaWriteEnabled)
	{
		blendDesc.RenderTarget[0].RenderTargetWriteMask |= D3D11_COLOR_WRITE_ENABLE_ALPHA;
	}

	DESIRE_TODO("Implement SetColorWriteEnabled()");
}

void RenderD3D11::SetDepthWriteEnabled(bool enabled)
{
	D3D11_DEPTH_STENCIL_DESC dsDesc = {};

	dsDesc.DepthWriteMask = enabled ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;

	DESIRE_TODO("Implement SetDepthWriteEnabled()");
}

void RenderD3D11::SetDepthTest(EDepthTest depthTest)
{
	D3D11_DEPTH_STENCIL_DESC dsDesc = {};

	dsDesc.DepthEnable = (depthTest != EDepthTest::DISABLED);
	switch(depthTest)
	{
		case EDepthTest::LESS:			dsDesc.DepthFunc = D3D11_COMPARISON_LESS; break;
		case EDepthTest::LESS_EQUAL:	dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL; break;
		case EDepthTest::GREATER:		dsDesc.DepthFunc = D3D11_COMPARISON_GREATER; break;
		case EDepthTest::GREATER_EQUAL:	dsDesc.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL; break;
		case EDepthTest::EQUAL:			dsDesc.DepthFunc = D3D11_COMPARISON_EQUAL; break;
		case EDepthTest::NOT_EQUAL:		dsDesc.DepthFunc = D3D11_COMPARISON_NOT_EQUAL; break;
	}

	DESIRE_TODO("Implement SetDepthTest()");
}

void RenderD3D11::SetCullMode(ECullMode cullMode)
{
	D3D11_RASTERIZER_DESC desc = {};

	switch(cullMode)
	{
		case IRender::ECullMode::NONE:	desc.CullMode = D3D11_CULL_NONE; break;
		case IRender::ECullMode::CCW:	desc.CullMode = D3D11_CULL_BACK; break;
		case IRender::ECullMode::CW:	desc.CullMode = D3D11_CULL_FRONT; break;
	}

	DESIRE_TODO("Implement SetCullMode()");
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
	for(Mesh::VertexDecl& decl : mesh->vertexDecl)
	{
		D3D11_INPUT_ELEMENT_DESC& elementDesc = renderData->vertexElementDesc[renderData->vertexElementDescCount];
		elementDesc = attribConversionTable[(size_t)decl.attrib];
		elementDesc.Format = attribTypeConversionTable[(size_t)decl.type][decl.count - 1];
		elementDesc.AlignedByteOffset = offset;

		renderData->vertexElementDescCount++;
		offset += decl.GetSizeInBytes();
	}

	D3D11_SUBRESOURCE_DATA initialIndexData = {};
	initialIndexData.pSysMem = mesh->indices;

	D3D11_SUBRESOURCE_DATA initialVertexData = {};
	initialVertexData.pSysMem = mesh->vertices;

	D3D11_BUFFER_DESC bufferDesc = {};
	switch(mesh->type)
	{
		case Mesh::EType::STATIC:
		{
			bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
			bufferDesc.CPUAccessFlags = 0;

			if(mesh->numIndices != 0)
			{
				bufferDesc.ByteWidth = mesh->GetSizeOfIndices();
				bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

				HRESULT hr = d3dDevice->CreateBuffer(&bufferDesc, &initialIndexData, &renderData->indexBuffer);
				ASSERT(SUCCEEDED(hr));
			}

			if(mesh->numVertices != 0)
			{
				bufferDesc.ByteWidth = mesh->GetSizeOfVertices();
				bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

				HRESULT hr = d3dDevice->CreateBuffer(&bufferDesc, &initialVertexData, &renderData->vertexBuffer);
				ASSERT(SUCCEEDED(hr));
			}
			break;
		}

		case Mesh::EType::DYNAMIC:
		{
			bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
			bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

			DynamicMesh *dynamicMesh = static_cast<DynamicMesh*>(mesh);
			if(dynamicMesh->maxNumOfIndices != 0)
			{
				bufferDesc.ByteWidth = dynamicMesh->GetMaxSizeOfIndices();
				bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

				HRESULT hr = d3dDevice->CreateBuffer(&bufferDesc, &initialIndexData, &renderData->indexBuffer);
				ASSERT(SUCCEEDED(hr));
			}

			if(dynamicMesh->maxNumOfVertices != 0)
			{
				bufferDesc.ByteWidth = dynamicMesh->GetMaxSizeOfVertices();
				bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

				HRESULT hr = d3dDevice->CreateBuffer(&bufferDesc, &initialVertexData, &renderData->vertexBuffer);
				ASSERT(SUCCEEDED(hr));
			}
			break;
		}
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

	const bool isVertexShader = shader->name.StartsWith("vs_");
	
	ShaderRenderDataD3D11 *renderData = new ShaderRenderDataD3D11();

	ID3DBlob *errorBlob = nullptr;
	HRESULT hr = D3DCompile(shader->data.data, shader->data.size, shader->name.c_str(), nullptr, nullptr, "main", isVertexShader ? "vs_5_0" : "ps_5_0", 0, 0, &renderData->shaderCode, &errorBlob);
	if(FAILED(hr))
	{
		if(errorBlob != nullptr)
		{
			LOG_ERROR("Shader compile error: %s", (char*)errorBlob->GetBufferPointer());
			errorBlob->Release();
		}

		delete renderData;
		shader->renderData = isVertexShader ? errorVertexShader->renderData : errorPixelShader->renderData;
		return;
	}

	if(isVertexShader)
	{
		hr = d3dDevice->CreateVertexShader(renderData->shaderCode->GetBufferPointer(), renderData->shaderCode->GetBufferSize(), nullptr, &renderData->vertexShader);
	}
	else
	{
		hr = d3dDevice->CreatePixelShader(renderData->shaderCode->GetBufferPointer(), renderData->shaderCode->GetBufferSize(), nullptr, &renderData->pixelShader);
	}
	ASSERT(SUCCEEDED(hr));

	renderData->ptr->SetPrivateData(WKPDID_D3DDebugObjectName, (UINT)shader->name.Length(), shader->name.c_str());

	ID3D11ShaderReflection *reflection = nullptr;
	hr = D3DReflect(renderData->shaderCode->GetBufferPointer(), renderData->shaderCode->GetBufferSize() , IID_ID3D11ShaderReflection, (void**)&reflection);
	if(FAILED(hr))
	{
		LOG_ERROR("D3DReflect failed 0x%08x\n", (uint32_t)hr);
	}

	D3D11_SHADER_DESC shaderDesc;
	hr = reflection->GetDesc(&shaderDesc);
	if(FAILED(hr))
	{
		LOG_ERROR("ID3D11ShaderReflection::GetDesc failed 0x%08x\n", (uint32_t)hr);
	}

	renderData->constantBuffers.reserve(shaderDesc.ConstantBuffers);
	renderData->constantBuffersData.reserve(shaderDesc.ConstantBuffers);
	for(uint32_t i = 0; i < shaderDesc.ConstantBuffers; ++i)
	{
		ID3D11ShaderReflectionConstantBuffer *cbuffer = reflection->GetConstantBufferByIndex(i);
		D3D11_SHADER_BUFFER_DESC shaderBufferDesc;
		hr = cbuffer->GetDesc(&shaderBufferDesc);
		ASSERT(SUCCEEDED(hr));

		// Create constant buffer
		renderData->constantBuffers.push_back(nullptr);

		D3D11_BUFFER_DESC bufferDesc = {};
		bufferDesc.ByteWidth = shaderBufferDesc.Size;
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		hr = d3dDevice->CreateBuffer(&bufferDesc, nullptr, &renderData->constantBuffers[i]);
		ASSERT(SUCCEEDED(hr));

		// Create constant buffer data
		renderData->constantBuffersData.push_back(MemoryBuffer(shaderBufferDesc.Size));

		for(uint32_t j = 0; j < shaderBufferDesc.Variables; ++j)
		{
			ID3D11ShaderReflectionVariable* var = cbuffer->GetVariableByIndex(j);
			ID3D11ShaderReflectionType *type = var->GetType();
			D3D11_SHADER_VARIABLE_DESC varDesc;
			hr = var->GetDesc(&varDesc);
			ASSERT(SUCCEEDED(hr));

			if(varDesc.uFlags & D3D_SVF_USED)
			{

			}
		}
	}

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

	D3D11_SUBRESOURCE_DATA subResourceData;
	subResourceData.pSysMem = texture->data.data;
	subResourceData.SysMemPitch = (UINT)(texture->data.size / texture->height);
	subResourceData.SysMemSlicePitch = 0;

	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = texture->width;
	textureDesc.Height = texture->height;
	textureDesc.MipLevels = texture->numMipMaps + 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = ConvertTextureFormat(texture->format);
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	DESIRE_TODO("Support Cube texture");
//	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

	ID3D11Texture2D *d3dTexture = nullptr;
	HRESULT hr = d3dDevice->CreateTexture2D(&textureDesc, &subResourceData, &d3dTexture);
	ASSERT(SUCCEEDED(hr));

	// Create texture view
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = textureDesc.MipLevels;
	hr = d3dDevice->CreateShaderResourceView(d3dTexture, &srvDesc, &renderData->textureSRV);
	ASSERT(SUCCEEDED(hr));

	DX_RELEASE(d3dTexture);

	// Create texture sampler
	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	hr = d3dDevice->CreateSamplerState(&samplerDesc, &renderData->samplerState);
	ASSERT(SUCCEEDED(hr));

	texture->renderData = renderData;
}

void RenderD3D11::Bind(RenderTarget *renderTarget)
{
	ASSERT(renderTarget != nullptr);

	if(renderTarget->renderData != nullptr)
	{
		// Already bound
		return;
	}

	RenderTargetRenderDataD3D11 *renderData = new RenderTargetRenderDataD3D11();

	const uint8_t textureCount = std::min<uint8_t>(renderTarget->GetTextureCount(), D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT);
	renderData->renderTargetViews.reserve(textureCount);
	for(uint8_t i = 0; i < textureCount; ++i)
	{
		// Bind texture
		const std::shared_ptr<Texture>& texture = renderTarget->GetTexture(i);
		ASSERT(texture->renderData == nullptr);

		TextureRenderDataD3D11 *textureRenderData = new TextureRenderDataD3D11();

		const bool isDepth = (texture->format == Texture::EFormat::D24S8);

		D3D11_TEXTURE2D_DESC textureDesc = {};
		textureDesc.Width = texture->width;
		textureDesc.Height = texture->height;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = ConvertTextureFormat(texture->format);
		textureDesc.SampleDesc.Count = 1;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = isDepth ? D3D11_BIND_DEPTH_STENCIL : (D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE);

		ID3D11Texture2D *d3dTexture = nullptr;
		HRESULT hr = d3dDevice->CreateTexture2D(&textureDesc, nullptr, &d3dTexture);
		ASSERT(SUCCEEDED(hr));

		if(isDepth)
		{
			hr = d3dDevice->CreateDepthStencilView(d3dTexture, nullptr, &renderData->depthStencilView);
			ASSERT(SUCCEEDED(hr));
		}
		else
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
			srvDesc.Format = textureDesc.Format;
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MipLevels = textureDesc.MipLevels;
			hr = d3dDevice->CreateShaderResourceView(d3dTexture, &srvDesc, &textureRenderData->textureSRV);
			ASSERT(SUCCEEDED(hr));

			D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc = {};
			renderTargetViewDesc.Format = textureDesc.Format;
			renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			renderTargetViewDesc.Texture2D.MipSlice = 0;
			ID3D11RenderTargetView *renderTargetView = nullptr;
			hr = d3dDevice->CreateRenderTargetView(d3dTexture, &renderTargetViewDesc, &renderTargetView);
			ASSERT(SUCCEEDED(hr));

			renderData->renderTargetViews.push_back(renderTargetView);
		}

		DX_RELEASE(d3dTexture);

		texture->renderData = textureRenderData;
	}

	renderTarget->renderData = renderData;
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
	DX_RELEASE(renderData->textureSRV);
	DX_RELEASE(renderData->samplerState);

	delete renderData;
	texture->renderData = nullptr;
}

void RenderD3D11::Unbind(RenderTarget *renderTarget)
{
	if(renderTarget == nullptr || renderTarget->renderData == nullptr)
	{
		// Not yet bound
		return;
	}

	RenderTargetRenderDataD3D11 *renderData = static_cast<RenderTargetRenderDataD3D11*>(renderTarget->renderData);

	for(ID3D11RenderTargetView *renderTargetView : renderData->renderTargetViews)
	{
		DX_RELEASE(renderTargetView);
	}
	renderData->renderTargetViews.clear();

	DX_RELEASE(renderData->depthStencilView);

	const uint8_t textureCount = std::min<uint8_t>(renderTarget->GetTextureCount(), D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT);
	for(uint8_t i = 0; i < textureCount; ++i)
	{
		Unbind(renderTarget->GetTexture(i).get());
	}

	delete renderData;
	renderTarget->renderData = nullptr;
}

void RenderD3D11::UpdateDynamicMesh(DynamicMesh *mesh)
{
	if(mesh == nullptr || mesh->renderData == nullptr)
	{
		// Not yet bound
		mesh->isIndexDataUpdateRequired = false;
		mesh->isVertexDataUpdateRequired = false;
		return;
	}

	MeshRenderDataD3D11 *renderData = static_cast<MeshRenderDataD3D11*>(mesh->renderData);

	if(mesh->isIndexDataUpdateRequired)
	{
		UpdateD3D11Resource(renderData->indexBuffer, mesh->indices, mesh->GetSizeOfIndices());
		mesh->isIndexDataUpdateRequired = false;
	}

	if(mesh->isVertexDataUpdateRequired)
	{
		UpdateD3D11Resource(renderData->vertexBuffer, mesh->vertices, mesh->GetSizeOfVertices());
		mesh->isVertexDataUpdateRequired = false;
	}
}

void RenderD3D11::SetMesh(Mesh *mesh)
{
	MeshRenderDataD3D11 *renderData = static_cast<MeshRenderDataD3D11*>(mesh->renderData);

	uint32_t indexByteOffset = renderData->indexOffset * sizeof(uint16_t);
	uint32_t vertexByteOffset = renderData->vertexOffset * mesh->stride;

	switch(mesh->type)
	{
		case Mesh::EType::STATIC:
		{
			if(activeMesh == mesh)
			{
				// No need to set the buffers again
				return;
			}
			break;
		}

		case Mesh::EType::DYNAMIC:
		{
			const DynamicMesh *dynamicMesh = static_cast<const DynamicMesh*>(mesh);
			indexByteOffset += dynamicMesh->indexOffset * sizeof(uint16_t);
			vertexByteOffset += dynamicMesh->vertexOffset * mesh->stride;
			break;
		}
	}

	deviceCtx->IASetIndexBuffer(renderData->indexBuffer, DXGI_FORMAT_R16_UINT, indexByteOffset);
	deviceCtx->IASetVertexBuffers(0, 1, &renderData->vertexBuffer, &mesh->stride, &vertexByteOffset);

	activeMesh = mesh;
}

void RenderD3D11::SetShadersFromMaterial(Material *material)
{
	// Vertex shader
	const ShaderRenderDataD3D11 *vertexShaderRenderData = static_cast<const ShaderRenderDataD3D11*>(material->vertexShader->renderData);
	deviceCtx->VSSetShader(vertexShaderRenderData->vertexShader, nullptr, 0);
	for(size_t i = 0; i < vertexShaderRenderData->constantBuffers.size(); ++i)
	{
/**/	DirectX::XMMATRIX matWorldView = DirectX::XMMatrixMultiply(matWorld, matView);
/**/	DirectX::XMMATRIX matWorldViewProj = DirectX::XMMatrixMultiply(matWorldView, matProj);
/**/	memcpy(vertexShaderRenderData->constantBuffersData[i].data, &matWorldViewProj.r[0], vertexShaderRenderData->constantBuffersData[i].size);

		deviceCtx->UpdateSubresource(vertexShaderRenderData->constantBuffers[i], 0, nullptr, vertexShaderRenderData->constantBuffersData[i].data, 0, 0);
	}
	deviceCtx->VSSetConstantBuffers(0, (UINT)vertexShaderRenderData->constantBuffers.size(), vertexShaderRenderData->constantBuffers.data());

	const MeshRenderDataD3D11 *meshRenderData = static_cast<const MeshRenderDataD3D11*>(activeMesh->renderData);
	ID3D11InputLayout *vertexLayout = nullptr;
	HRESULT hr = d3dDevice->CreateInputLayout(meshRenderData->vertexElementDesc.get(), meshRenderData->vertexElementDescCount, vertexShaderRenderData->shaderCode->GetBufferPointer(), vertexShaderRenderData->shaderCode->GetBufferSize(), &vertexLayout);
	ASSERT(SUCCEEDED(hr));

	deviceCtx->IASetInputLayout(vertexLayout);
	DX_RELEASE(vertexLayout);

	// Pixel shader
	const ShaderRenderDataD3D11 *pixelShaderRenderData = static_cast<const ShaderRenderDataD3D11*>(material->pixelShader->renderData);
	deviceCtx->PSSetShader(pixelShaderRenderData->pixelShader, nullptr, 0);

	for(size_t i = 0; i < pixelShaderRenderData->constantBuffers.size(); ++i)
	{
		deviceCtx->UpdateSubresource(pixelShaderRenderData->constantBuffers[i], 0, nullptr, pixelShaderRenderData->constantBuffersData[i].data, 0, 0);
	}
	deviceCtx->PSSetConstantBuffers(0, (UINT)pixelShaderRenderData->constantBuffers.size(), pixelShaderRenderData->constantBuffers.data());
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

void RenderD3D11::SetViewport(uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
	const D3D11_VIEWPORT vp = { (float)x, (float)y, (float)width, (float)height, 0.0f, 1.0f };
	deviceCtx->RSSetViewports(1, &vp);
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

void RenderD3D11::UpdateD3D11Resource(ID3D11Resource *resource, const void *data, size_t size)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr = deviceCtx->Map(resource, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(hr))
	{
		LOG_WARNING("Failed to map index buffer");
		return;
	}

	memcpy(mappedResource.pData, data, size);
	deviceCtx->Unmap(resource, 0);
}

DXGI_FORMAT RenderD3D11::ConvertTextureFormat(Texture::EFormat textureFormat)
{
	switch(textureFormat)
	{
		case Texture::EFormat::UNKNOWN:		return DXGI_FORMAT_UNKNOWN;
		case Texture::EFormat::R8:			return DXGI_FORMAT_R8_UNORM;
		case Texture::EFormat::RG8:			return DXGI_FORMAT_R8G8_UNORM;
		case Texture::EFormat::RGB8:		return DXGI_FORMAT_UNKNOWN;
		case Texture::EFormat::RGBA8:		return DXGI_FORMAT_R8G8B8A8_UNORM;
		case Texture::EFormat::RGBA32F:		return DXGI_FORMAT_R32G32B32A32_FLOAT;
		case Texture::EFormat::D24S8:		return DXGI_FORMAT_D24_UNORM_S8_UINT;
	}

	return DXGI_FORMAT_UNKNOWN;
}
