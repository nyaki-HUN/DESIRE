#include "stdafx_Direct3D11.h"
#include "Direct3D11Render.h"

#include "DirectXMathExt.h"
#include "MeshRenderDataD3D11.h"
#include "RenderTargetRenderDataD3D11.h"
#include "ShaderRenderDataD3D11.h"
#include "TextureRenderDataD3D11.h"

#include "Engine/Application/OSWindow.h"

#include "Engine/Core/FS/FileSystem.h"
#include "Engine/Core/Math/Matrix4.h"
#include "Engine/Core/String/StackString.h"

#include "Engine/Render/Material.h"
#include "Engine/Render/RenderTarget.h"
#include "Engine/Render/View.h"

#include "Engine/Resource/Mesh.h"
#include "Engine/Resource/Shader.h"
#include "Engine/Resource/Texture.h"

#include <d3dcompiler.h>

#define DX_RELEASE(ptr)		\
	if(ptr != nullptr)		\
	{						\
		ptr->Release();		\
		ptr = nullptr;		\
	}

#if defined(_DEBUG)
	#define DX_CHECK_HRESULT(hr)		ASSERT(SUCCEEDED(hr))
#else
	#define DX_CHECK_HRESULT(hr)		DESIRE_UNUSED(hr)
#endif

Direct3D11Render::Direct3D11Render()
{
	const char vs_screenSpaceQuad[] =
	{
		"struct v2f\n"
		"{\n"
		"	float4 pos : SV_Position;\n"
		"	float2 uv : TEXCOORD0;\n"
		"};\n"
		"v2f main(uint idx : SV_VertexID)\n"
		"{\n"
		"	v2f Out;\n"
		"	Out.uv = float2(idx & 1, idx >> 1);\n"
		"	Out.pos = float4((Out.uv.x - 0.5) * 2.0, -(Out.uv.y - 0.5) * 2.0, 0.0, 1.0);\n"
		"	return Out;\n"
		"}\n"
	};
	screenSpaceQuadVertexShader->data = MemoryBuffer::CreateFromDataCopy(vs_screenSpaceQuad, sizeof(vs_screenSpaceQuad));

	const char vs_error[] =
	{
		"cbuffer CB : register(b0)\n"
		"{\n"
		"	float4x4 matWorldViewProj;\n"
		"};\n"
		"float4 main(float3 pos : POSITION) : SV_Position\n"
		"{\n"
		"	float4 position = mul(matWorldViewProj, float4(pos, 1.0));\n"
		"	return position;\n"
		"}"
	};
	const char ps_error[] =
	{
		"float3 main() : SV_Target\n"
		"{\n"
		"	return float3(1, 0, 1);\n"
		"}"
	};
	errorVertexShader = std::make_unique<Shader>("vs_error");
	errorVertexShader->data = MemoryBuffer::CreateFromDataCopy(vs_error, sizeof(vs_error));
	errorPixelShader = std::make_unique<Shader>("ps_error");
	errorPixelShader->data = MemoryBuffer::CreateFromDataCopy(ps_error, sizeof(ps_error));

	// Stencil test parameters
	depthStencilDesc.StencilEnable = FALSE;
	depthStencilDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	depthStencilDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	// Stencil operations if pixel is front-facing
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	// Stencil operations if pixel is back-facing
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.DepthClipEnable = TRUE;
	rasterizerDesc.MultisampleEnable = TRUE;
	rasterizerDesc.AntialiasedLineEnable = TRUE;

	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	matWorld = DirectX::XMMatrixIdentity();
	matView = DirectX::XMMatrixIdentity();
	matProj = DirectX::XMMatrixIdentity();
}

Direct3D11Render::~Direct3D11Render()
{
}

void Direct3D11Render::Init(OSWindow* pMainWindow)
{
	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChainDesc.BufferDesc.Width = pMainWindow->GetWidth();
	swapChainDesc.BufferDesc.Height = pMainWindow->GetHeight();
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 1;
	swapChainDesc.OutputWindow = (HWND)pMainWindow->GetHandle();
	swapChainDesc.Windowed = TRUE;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	HRESULT hr = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
#if defined(_DEBUG)
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
	if(!initialized)
	{
		return;
	}

	// Set the default topology when there is no active mesh
	deviceCtx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	CreateBackBuffer(pMainWindow->GetWidth(), pMainWindow->GetHeight());
	SetDefaultRenderStates();

	Bind(screenSpaceQuadVertexShader.get());
	Bind(errorVertexShader.get());
	Bind(errorPixelShader.get());
}

void Direct3D11Render::UpdateRenderWindow(OSWindow* pWindow)
{
	if(!initialized)
	{
		return;
	}

	ID3D11RenderTargetView* nullViews[] = { nullptr };
	deviceCtx->OMSetRenderTargets(1, nullViews, nullptr);
	DX_RELEASE(backBufferDepthStencilView);
	DX_RELEASE(backBufferRenderTargetView);
	deviceCtx->Flush();

	HRESULT hr = swapChain->ResizeBuffers(0, pWindow->GetWidth(), pWindow->GetHeight(), DXGI_FORMAT_UNKNOWN, 0);
	if(hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
	{
		// Have to destroy the device, swapchain, and all resources and recreate them to recover from this case
	}

	DX_CHECK_HRESULT(hr);

	CreateBackBuffer(pWindow->GetWidth(), pWindow->GetHeight());
}

void Direct3D11Render::Kill()
{
	initialized = false;

	for(auto& pair : depthStencilStateCache)
	{
		DX_RELEASE(pair.second);
	}
	depthStencilStateCache.clear();
	pActiveDepthStencilState = nullptr;

	for(auto& pair : rasterizerStateCache)
	{
		DX_RELEASE(pair.second);
	}
	rasterizerStateCache.clear();
	pActiveRasterizerState = nullptr;

	for(auto& pair : blendStateCache)
	{
		DX_RELEASE(pair.second);
	}
	blendStateCache.clear();
	pActiveBlendState = nullptr;

	for(auto& pair : inputLayoutCache)
	{
		DX_RELEASE(pair.second);
	}
	inputLayoutCache.clear();
	pActiveInputLayout = nullptr;

	for(auto& pair : samplerStateCache)
	{
		DX_RELEASE(pair.second);
	}
	samplerStateCache.clear();
	memset(activeSamplerStates, 0, sizeof(activeSamplerStates));

	pActiveWindow = nullptr;
	pActiveMesh = nullptr;
	pActiveVertexShader = nullptr;
	pActiveFragmentShader = nullptr;

	Unbind(errorVertexShader.get());
	Unbind(errorPixelShader.get());
	Unbind(screenSpaceQuadVertexShader.get());

	DX_RELEASE(backBufferDepthStencilView);
	DX_RELEASE(backBufferRenderTargetView);
}

void Direct3D11Render::AppendShaderFilenameWithPath(WritableString& outString, const String& shaderFilename) const
{
	outString += "data/shaders/hlsl/";
	outString += shaderFilename;
	outString += ".hlsl";
}

void Direct3D11Render::BeginFrame(OSWindow* pWindow)
{
	pActiveWindow = pWindow;
	SetViewport(0, 0, pWindow->GetWidth(), pWindow->GetHeight());

	deviceCtx->ClearRenderTargetView(backBufferRenderTargetView, clearColor);
	deviceCtx->ClearDepthStencilView(backBufferDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void Direct3D11Render::EndFrame()
{
	swapChain->Present(1, 0);
}

void Direct3D11Render::SetView(View* pView)
{
	if(pView == pActiveView)
	{
		return;
	}

	if(pView != nullptr)
	{
		RenderTarget* rt = pView->GetRenderTarget();
		if(rt->renderData == nullptr)
		{
			Bind(rt);
		}

		RenderTargetRenderDataD3D11* pRenderData = static_cast<RenderTargetRenderDataD3D11*>(rt->renderData);
		deviceCtx->OMSetRenderTargets(static_cast<UINT>(pRenderData->renderTargetViews.size()), pRenderData->renderTargetViews.data(), pRenderData->depthStencilView);
		SetViewport(pView->GetPosX(), pView->GetPosY(), pView->GetWidth(), pView->GetHeight());

		for(ID3D11RenderTargetView* renderTargetView : pRenderData->renderTargetViews)
		{
			deviceCtx->ClearRenderTargetView(renderTargetView, clearColor);
		}
		if(pRenderData->depthStencilView != nullptr)
		{
			deviceCtx->ClearDepthStencilView(pRenderData->depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		}
	}
	else
	{
		deviceCtx->OMSetRenderTargets(1, &backBufferRenderTargetView, backBufferDepthStencilView);
		SetViewport(0, 0, pActiveWindow->GetWidth(), pActiveWindow->GetHeight());
	}

	pActiveView = pView;
}

void Direct3D11Render::SetWorldMatrix(const Matrix4& matrix)
{
	matWorld.r[0] = GetXMVECTOR(matrix.col0);
	matWorld.r[1] = GetXMVECTOR(matrix.col1);
	matWorld.r[2] = GetXMVECTOR(matrix.col2);
	matWorld.r[3] = GetXMVECTOR(matrix.col3);
}

void Direct3D11Render::SetViewProjectionMatrices(const Matrix4& viewMatrix, const Matrix4& projMatrix)
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

void Direct3D11Render::SetScissor(uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
	if(x == 0 && y == 0 && width == 0 && height == 0)
	{
		rasterizerDesc.ScissorEnable = FALSE;
	}
	else
	{
		rasterizerDesc.ScissorEnable = TRUE;

		const D3D11_RECT rect = { x, y, x + width, y + height };
		deviceCtx->RSSetScissorRects(1, &rect);
	}
}

void Direct3D11Render::SetClearColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	clearColor[0] = r / 255.0f;
	clearColor[1] = g / 255.0f;
	clearColor[2] = b / 255.0f;
	clearColor[3] = a / 255.0f;
}

void Direct3D11Render::SetColorWriteEnabled(bool r, bool g, bool b, bool a)
{
	ASSERT(!blendDesc.IndependentBlendEnable && "Independent render target blend states are not supported (only the RenderTarget[0] members are used)");

	if(r)
	{
		blendDesc.RenderTarget[0].RenderTargetWriteMask |= D3D11_COLOR_WRITE_ENABLE_RED;
	}
	else
	{
		blendDesc.RenderTarget[0].RenderTargetWriteMask &= ~D3D11_COLOR_WRITE_ENABLE_RED;
	}

	if(g)
	{
		blendDesc.RenderTarget[0].RenderTargetWriteMask |= D3D11_COLOR_WRITE_ENABLE_GREEN;
	}
	else
	{
		blendDesc.RenderTarget[0].RenderTargetWriteMask &= ~D3D11_COLOR_WRITE_ENABLE_GREEN;
	}

	if(b)
	{
		blendDesc.RenderTarget[0].RenderTargetWriteMask |= D3D11_COLOR_WRITE_ENABLE_BLUE;
	}
	else
	{
		blendDesc.RenderTarget[0].RenderTargetWriteMask &= ~D3D11_COLOR_WRITE_ENABLE_BLUE;
	}

	if(a)
	{
		blendDesc.RenderTarget[0].RenderTargetWriteMask |= D3D11_COLOR_WRITE_ENABLE_ALPHA;
	}
	else
	{
		blendDesc.RenderTarget[0].RenderTargetWriteMask &= ~D3D11_COLOR_WRITE_ENABLE_ALPHA;
	}
}

void Direct3D11Render::SetDepthWriteEnabled(bool enabled)
{
	depthStencilDesc.DepthWriteMask = enabled ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
}

void Direct3D11Render::SetDepthTest(EDepthTest depthTest)
{
	switch(depthTest)
	{
		case EDepthTest::Disabled:
			depthStencilDesc.DepthEnable = FALSE;
			return;

		case EDepthTest::Less:			depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS; break;
		case EDepthTest::LessEqual:		depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL; break;
		case EDepthTest::Greater:		depthStencilDesc.DepthFunc = D3D11_COMPARISON_GREATER; break;
		case EDepthTest::GreaterEqual:	depthStencilDesc.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL; break;
		case EDepthTest::Equal:			depthStencilDesc.DepthFunc = D3D11_COMPARISON_EQUAL; break;
		case EDepthTest::NotEqual:		depthStencilDesc.DepthFunc = D3D11_COMPARISON_NOT_EQUAL; break;
	}

	depthStencilDesc.DepthEnable = TRUE;
}

void Direct3D11Render::SetCullMode(ECullMode cullMode)
{
	switch(cullMode)
	{
		case ECullMode::None:	rasterizerDesc.CullMode = D3D11_CULL_NONE; break;
		case ECullMode::CCW:	rasterizerDesc.CullMode = D3D11_CULL_BACK; break;
		case ECullMode::CW:		rasterizerDesc.CullMode = D3D11_CULL_FRONT; break;
	}
}

void Direct3D11Render::SetBlendModeSeparated(EBlend srcBlendRGB, EBlend destBlendRGB, EBlendOp blendOpRGB, EBlend srcBlendAlpha, EBlend destBlendAlpha, EBlendOp blendOpAlpha)
{
	ASSERT(!blendDesc.IndependentBlendEnable && "Independent render target blend states are not supported (only the RenderTarget[0] members are used)");

	blendDesc.RenderTarget[0].BlendEnable = TRUE;

	static const D3D11_BLEND blendConversionTable[] =
	{
		D3D11_BLEND_ZERO,				// EBlend::Zero
		D3D11_BLEND_ONE,				// EBlend::One
		D3D11_BLEND_SRC_COLOR,			// EBlend::SrcColor
		D3D11_BLEND_INV_SRC_COLOR,		// EBlend::InvSrcColor
		D3D11_BLEND_SRC_ALPHA,			// EBlend::SrcAlpha
		D3D11_BLEND_INV_SRC_ALPHA,		// EBlend::InvSrcAlpha
		D3D11_BLEND_DEST_ALPHA,			// EBlend::DestAlpha
		D3D11_BLEND_INV_DEST_ALPHA,		// EBlend::InvDestAlpha
		D3D11_BLEND_DEST_COLOR,			// EBlend::DestColor
		D3D11_BLEND_INV_DEST_COLOR,		// EBlend::InvDestColor
		D3D11_BLEND_SRC_ALPHA_SAT,		// EBlend::SrcAlphaSat
		D3D11_BLEND_BLEND_FACTOR,		// EBlend::BlendFactor
		D3D11_BLEND_INV_BLEND_FACTOR	// EBlend::InvBlendFactor
	};
	DESIRE_CHECK_ARRAY_SIZE(blendConversionTable, EBlend::InvBlendFactor + 1);

	blendDesc.RenderTarget[0].SrcBlend = blendConversionTable[(size_t)srcBlendRGB];
	blendDesc.RenderTarget[0].DestBlend = blendConversionTable[(size_t)destBlendRGB];
	blendDesc.RenderTarget[0].SrcBlendAlpha = blendConversionTable[(size_t)srcBlendAlpha];
	blendDesc.RenderTarget[0].DestBlendAlpha = blendConversionTable[(size_t)destBlendAlpha];

	static const D3D11_BLEND_OP equationConversionTable[] =
	{
		D3D11_BLEND_OP_ADD,				// EBlendOp::Add
		D3D11_BLEND_OP_SUBTRACT,		// EBlendOp::Subtract
		D3D11_BLEND_OP_REV_SUBTRACT,	// EBlendOp::RevSubtract
		D3D11_BLEND_OP_MIN,				// EBlendOp::Min
		D3D11_BLEND_OP_MAX				// EBlendOp::Max
	};
	DESIRE_CHECK_ARRAY_SIZE(equationConversionTable, EBlendOp::Max + 1);

	blendDesc.RenderTarget[0].BlendOp = equationConversionTable[(size_t)blendOpRGB];
	blendDesc.RenderTarget[0].BlendOpAlpha = equationConversionTable[(size_t)blendOpAlpha];
}

void Direct3D11Render::SetBlendModeDisabled()
{
	blendDesc.RenderTarget[0].BlendEnable = FALSE;
}

void Direct3D11Render::Bind(Mesh* pMesh)
{
	ASSERT(pMesh != nullptr);

	if(pMesh->pRenderData != nullptr)
	{
		// Already bound
		return;
	}

	MeshRenderDataD3D11* pRenderData = new MeshRenderDataD3D11();

	const Array<Mesh::VertexLayout>& vertexLayout = pMesh->GetVertexLayout();
	ASSERT(vertexLayout.Size() <= 9 && "It is possible to encode maximum of 9 vertex layouts into 64-bit");
	for(size_t i = 0; i < vertexLayout.Size(); ++i)
	{
		const Mesh::VertexLayout& layout = vertexLayout[i];
		pRenderData->vertexLayoutKey |= (uint64_t)layout.attrib			<< (i * 7 + 0);	// 4 bits
		pRenderData->vertexLayoutKey |= (uint64_t)layout.type			<< (i * 7 + 4);	// 1 bit
		pRenderData->vertexLayoutKey |= (uint64_t)(layout.count - 1)	<< (i * 7 + 5);	// 2 bits [0, 3]
	}

	D3D11_BUFFER_DESC bufferDesc = {};
	switch(pMesh->GetType())
	{
		case Mesh::EType::Static:
			bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
			bufferDesc.CPUAccessFlags = 0;
			break;

		case Mesh::EType::Dynamic:
			bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
			bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			break;
	}

	if(pMesh->numIndices != 0)
	{
		bufferDesc.ByteWidth = pMesh->GetSizeOfIndexData();
		bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA initialIndexData = {};
		initialIndexData.pSysMem = pMesh->indices.get();
		HRESULT hr = d3dDevice->CreateBuffer(&bufferDesc, &initialIndexData, &pRenderData->indexBuffer);
		DX_CHECK_HRESULT(hr);
	}

	bufferDesc.ByteWidth = pMesh->GetSizeOfVertexData();
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA initialVertexData = {};
	initialVertexData.pSysMem = pMesh->vertices.get();
	HRESULT hr = d3dDevice->CreateBuffer(&bufferDesc, &initialVertexData, &pRenderData->vertexBuffer);
	DX_CHECK_HRESULT(hr);

	pMesh->pRenderData = pRenderData;
}

void Direct3D11Render::Bind(Shader* pShader)
{
	ASSERT(pShader != nullptr);

	if(pShader->renderData != nullptr)
	{
		// Already bound
		return;
	}

	const bool isVertexShader = pShader->name.StartsWith("vs_");

	ShaderRenderDataD3D11* pRenderData = new ShaderRenderDataD3D11();

	D3D_SHADER_MACRO defines[32] = {};
	ASSERT(pShader->defines.size() < DESIRE_ASIZEOF(defines));
	D3D_SHADER_MACRO* definePtr = &defines[0];
	for(const String& define : pShader->defines)
	{
		definePtr->Name = define.Str();
		definePtr->Definition = "1";
		definePtr++;
	}

	StackString<DESIRE_MAX_PATH_LEN> filenameWithPath = FileSystem::Get()->GetAppDirectory();
	AppendShaderFilenameWithPath(filenameWithPath, pShader->name);

	UINT compileFlags = 0;
/**/compileFlags = D3DCOMPILE_SKIP_OPTIMIZATION;

	ID3DBlob* pErrorBlob = nullptr;
	HRESULT hr = D3DCompile(pShader->data.ptr.get()	// pSrcData
		, pShader->data.size						// SrcDataSize
		, filenameWithPath.Str()					// pSourceName
		, defines									// pDefines
		, D3D_COMPILE_STANDARD_FILE_INCLUDE			// pInclude
		, "main"									// pEntrypoint
		, isVertexShader ? "vs_5_0" : "ps_5_0"		// pTarget
		, compileFlags								// D3DCOMPILE flags
		, 0											// D3DCOMPILE_EFFECT flags
		, &pRenderData->shaderCode					// ppCode
		, &pErrorBlob);								// ppErrorMsgs
	if(FAILED(hr))
	{
		if(pErrorBlob != nullptr)
		{
			LOG_ERROR("Shader compile error:\n%s", static_cast<char*>(pErrorBlob->GetBufferPointer()));
			DX_RELEASE(pErrorBlob);
		}

		delete pRenderData;
		pShader->renderData = isVertexShader ? errorVertexShader->renderData : errorPixelShader->renderData;
		return;
	}

	if(isVertexShader)
	{
		hr = d3dDevice->CreateVertexShader(pRenderData->shaderCode->GetBufferPointer(), pRenderData->shaderCode->GetBufferSize(), nullptr, &pRenderData->vertexShader);
	}
	else
	{
		hr = d3dDevice->CreatePixelShader(pRenderData->shaderCode->GetBufferPointer(), pRenderData->shaderCode->GetBufferSize(), nullptr, &pRenderData->pixelShader);
	}
	DX_CHECK_HRESULT(hr);

	pRenderData->ptr->SetPrivateData(WKPDID_D3DDebugObjectName, (UINT)pShader->name.Length(), pShader->name.Str());

	ID3D11ShaderReflection* pReflection = nullptr;
	hr = D3DReflect(pRenderData->shaderCode->GetBufferPointer(), pRenderData->shaderCode->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&pReflection);
	if(FAILED(hr))
	{
		LOG_ERROR("D3DReflect failed 0x%08x\n", (uint32_t)hr);
	}

	D3D11_SHADER_DESC shaderDesc;
	hr = pReflection->GetDesc(&shaderDesc);
	if(FAILED(hr))
	{
		LOG_ERROR("ID3D11ShaderReflection::GetDesc failed 0x%08x\n", (uint32_t)hr);
	}

	pRenderData->constantBuffers.resize(shaderDesc.ConstantBuffers);
	pRenderData->constantBuffersData.resize(shaderDesc.ConstantBuffers);
	for(uint32_t i = 0; i < shaderDesc.ConstantBuffers; ++i)
	{
		ID3D11ShaderReflectionConstantBuffer* cbuffer = pReflection->GetConstantBufferByIndex(i);
		D3D11_SHADER_BUFFER_DESC shaderBufferDesc;
		hr = cbuffer->GetDesc(&shaderBufferDesc);
		DX_CHECK_HRESULT(hr);

		// Create constant buffer
		D3D11_BUFFER_DESC bufferDesc = {};
		bufferDesc.ByteWidth = shaderBufferDesc.Size;
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		hr = d3dDevice->CreateBuffer(&bufferDesc, nullptr, &pRenderData->constantBuffers[i]);
		DX_CHECK_HRESULT(hr);

		// Create constant buffer data
		ShaderRenderDataD3D11::ConstantBufferData& bufferData = pRenderData->constantBuffersData[i];
		bufferData.data = MemoryBuffer(shaderBufferDesc.Size);

		for(uint32_t j = 0; j < shaderBufferDesc.Variables; ++j)
		{
			ID3D11ShaderReflectionVariable* shaderVar = cbuffer->GetVariableByIndex(j);
			D3D11_SHADER_VARIABLE_DESC varDesc;
			hr = shaderVar->GetDesc(&varDesc);
			DX_CHECK_HRESULT(hr);

			if((varDesc.uFlags & D3D_SVF_USED) == 0)
			{
				continue;
			}

			ID3D11ShaderReflectionType* type = shaderVar->GetType();
			D3D11_SHADER_TYPE_DESC typeDesc;
			hr = type->GetDesc(&typeDesc);
			DX_CHECK_HRESULT(hr);

			if( typeDesc.Class != D3D_SVC_SCALAR &&
				typeDesc.Class != D3D_SVC_VECTOR &&
				typeDesc.Class != D3D_SVC_MATRIX_COLUMNS)
			{
				continue;
			}

			const HashedString key = HashedString::CreateFromString(String(varDesc.Name, strlen(varDesc.Name)));
			bufferData.variableOffsetSizePairs.Insert(key, std::make_pair(varDesc.StartOffset, varDesc.Size));
		}
	}

	DX_RELEASE(pReflection);

	pShader->renderData = pRenderData;
}

void Direct3D11Render::Bind(Texture* texture)
{
	ASSERT(texture != nullptr);

	if(texture->renderData != nullptr)
	{
		// Already bound
		return;
	}

	TextureRenderDataD3D11* pRenderData = new TextureRenderDataD3D11();

	const bool isRenderTarget = (texture->data.ptr == nullptr);

	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = texture->width;
	textureDesc.Height = texture->height;
	textureDesc.MipLevels = texture->numMipMaps + 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = GetTextureFormat(texture);
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	if(texture->IsDepthFormat())
	{
		textureDesc.BindFlags |= D3D11_BIND_DEPTH_STENCIL;
	}
	else if(isRenderTarget)
	{
		textureDesc.BindFlags |= D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		if(texture->numMipMaps > 0)
		{
			textureDesc.MiscFlags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;
		}
	}
	else
	{
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	}

	{
		DESIRE_TODO("Support Cube texture");
//		textureDesc.MiscFlags |= D3D11_RESOURCE_MISC_TEXTURECUBE;
	}

	if(isRenderTarget)
	{
		HRESULT hr = d3dDevice->CreateTexture2D(&textureDesc, nullptr, &pRenderData->texture2D);
		DX_CHECK_HRESULT(hr);
	}
	else
	{
		std::unique_ptr<D3D11_SUBRESOURCE_DATA[]> subResourceData = std::make_unique<D3D11_SUBRESOURCE_DATA[]>(textureDesc.MipLevels * textureDesc.ArraySize);
		ASSERT(textureDesc.MipLevels * textureDesc.ArraySize == 1 && "TODO: Set initial data properly in the loop below");
		for(size_t i = 0; i < textureDesc.MipLevels * textureDesc.ArraySize; ++i)
		{
			subResourceData[i].pSysMem = texture->data.ptr.get();
			subResourceData[i].SysMemPitch = (UINT)(texture->data.size / texture->height);
			subResourceData[i].SysMemSlicePitch = 0;
		}

		HRESULT hr = d3dDevice->CreateTexture2D(&textureDesc, subResourceData.get(), &pRenderData->texture2D);
		DX_CHECK_HRESULT(hr);
	}

	if(textureDesc.BindFlags & D3D11_BIND_SHADER_RESOURCE)
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = textureDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = textureDesc.MipLevels;
		HRESULT hr = d3dDevice->CreateShaderResourceView(pRenderData->texture2D, &srvDesc, &pRenderData->textureSRV);
		DX_CHECK_HRESULT(hr);
	}

	texture->renderData = pRenderData;
}

void Direct3D11Render::Bind(RenderTarget* renderTarget)
{
	ASSERT(renderTarget != nullptr);

	if(renderTarget->renderData != nullptr)
	{
		// Already bound
		return;
	}

	RenderTargetRenderDataD3D11* pRenderData = new RenderTargetRenderDataD3D11();

	const uint8_t textureCount = std::min<uint8_t>(renderTarget->GetTextureCount(), D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT);
	pRenderData->renderTargetViews.reserve(textureCount);
	for(uint8_t i = 0; i < textureCount; ++i)
	{
		// Bind texture
		const std::shared_ptr<Texture>& texture = renderTarget->GetTexture(i);
		ASSERT(texture->renderData == nullptr);
		Bind(texture.get());

		TextureRenderDataD3D11* textureRenderData = static_cast<TextureRenderDataD3D11*>(texture->renderData);

		if(texture->IsDepthFormat())
		{
			ASSERT(pRenderData->depthStencilView == nullptr && "RenderTargets can have only 1 depth attachment");

			HRESULT hr = d3dDevice->CreateDepthStencilView(textureRenderData->texture2D, nullptr, &pRenderData->depthStencilView);
			DX_CHECK_HRESULT(hr);
		}
		else
		{
			D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc = {};
			renderTargetViewDesc.Format = GetTextureFormat(texture.get());
			renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			renderTargetViewDesc.Texture2D.MipSlice = 0;
			ID3D11RenderTargetView* renderTargetView = nullptr;
			HRESULT hr = d3dDevice->CreateRenderTargetView(textureRenderData->texture2D, &renderTargetViewDesc, &renderTargetView);
			DX_CHECK_HRESULT(hr);

			pRenderData->renderTargetViews.push_back(renderTargetView);
		}
	}

	renderTarget->renderData = pRenderData;
}

void Direct3D11Render::Unbind(Mesh* pMesh)
{
	if(pMesh == nullptr || pMesh->pRenderData == nullptr)
	{
		// Not yet bound
		return;
	}

	MeshRenderDataD3D11* pRenderData = static_cast<MeshRenderDataD3D11*>(pMesh->pRenderData);
	DX_RELEASE(pRenderData->indexBuffer);
	DX_RELEASE(pRenderData->vertexBuffer);

	delete pRenderData;
	pMesh->pRenderData = nullptr;

	if(pActiveMesh == pMesh)
	{
		pActiveMesh = nullptr;
	}
}

void Direct3D11Render::Unbind(Shader* pShader)
{
	if(pShader == nullptr || pShader->renderData == nullptr)
	{
		// Not yet bound
		return;
	}

	if((pShader->renderData == errorVertexShader->renderData || pShader->renderData == errorPixelShader->renderData) &&
		pShader != errorVertexShader.get() &&
		pShader != errorPixelShader.get())
	{
		pShader->renderData = nullptr;
		return;
	}

	ShaderRenderDataD3D11* pRenderData = static_cast<ShaderRenderDataD3D11*>(pShader->renderData);
	DX_RELEASE(pRenderData->ptr);
	DX_RELEASE(pRenderData->shaderCode);

	for(ID3D11Buffer* buffer : pRenderData->constantBuffers)
	{
		DX_RELEASE(buffer)
	}
	pRenderData->constantBuffers.clear();
	pRenderData->constantBuffersData.clear();

	delete pRenderData;
	pShader->renderData = nullptr;

	if(pActiveVertexShader == pShader)
	{
		pActiveVertexShader = nullptr;
	}
	if(pActiveFragmentShader == pShader)
	{
		pActiveFragmentShader = nullptr;
	}
}

void Direct3D11Render::Unbind(Texture* texture)
{
	if(texture == nullptr || texture->renderData == nullptr)
	{
		// Not yet bound
		return;
	}

	TextureRenderDataD3D11* pRenderData = static_cast<TextureRenderDataD3D11*>(texture->renderData);
	DX_RELEASE(pRenderData->textureSRV);
	DX_RELEASE(pRenderData->texture2D);

	delete pRenderData;
	texture->renderData = nullptr;
}

void Direct3D11Render::Unbind(RenderTarget* renderTarget)
{
	if(renderTarget == nullptr || renderTarget->renderData == nullptr)
	{
		// Not yet bound
		return;
	}

	RenderTargetRenderDataD3D11* pRenderData = static_cast<RenderTargetRenderDataD3D11*>(renderTarget->renderData);

	for(ID3D11RenderTargetView* renderTargetView : pRenderData->renderTargetViews)
	{
		DX_RELEASE(renderTargetView);
	}
	pRenderData->renderTargetViews.clear();

	DX_RELEASE(pRenderData->depthStencilView);

	const uint8_t textureCount = std::min<uint8_t>(renderTarget->GetTextureCount(), D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT);
	for(uint8_t i = 0; i < textureCount; ++i)
	{
		Unbind(renderTarget->GetTexture(i).get());
	}

	delete pRenderData;
	renderTarget->renderData = nullptr;
}

void Direct3D11Render::CreateBackBuffer(uint32_t width, uint32_t height)
{
	// Create back buffer render target view
	ID3D11Texture2D* backBufferTexture = nullptr;
	HRESULT hr = swapChain->GetBuffer(0, IID_ID3D11Texture2D, (void**)&backBufferTexture);
	DX_CHECK_HRESULT(hr);
	hr = d3dDevice->CreateRenderTargetView(backBufferTexture, nullptr, &backBufferRenderTargetView);
	DX_CHECK_HRESULT(hr);
	DX_RELEASE(backBufferTexture);

	// Create back buffer depth stencil view
	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	ID3D11Texture2D* depthStencilTexture = nullptr;
	hr = d3dDevice->CreateTexture2D(&textureDesc, nullptr, &depthStencilTexture);
	DX_CHECK_HRESULT(hr);

	hr = d3dDevice->CreateDepthStencilView(depthStencilTexture, nullptr, &backBufferDepthStencilView);
	DX_CHECK_HRESULT(hr);

	DX_RELEASE(depthStencilTexture);

	deviceCtx->OMSetRenderTargets(1, &backBufferRenderTargetView, backBufferDepthStencilView);
}

void Direct3D11Render::SetViewport(uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
	const D3D11_VIEWPORT vp = { (float)x, (float)y, (float)width, (float)height, 0.0f, 1.0f };
	deviceCtx->RSSetViewports(1, &vp);
}

void Direct3D11Render::SetMesh(Mesh* pMesh)
{
	if(pMesh != nullptr)
	{
		MeshRenderDataD3D11* pRenderData = static_cast<MeshRenderDataD3D11*>(pMesh->pRenderData);
		const uint32_t indexByteOffset = pRenderData->indexOffset * sizeof(uint16_t);
		const uint32_t vertexByteOffset = pRenderData->vertexOffset * pMesh->stride;
		deviceCtx->IASetIndexBuffer(pRenderData->indexBuffer, DXGI_FORMAT_R16_UINT, indexByteOffset);
		deviceCtx->IASetVertexBuffers(0, 1, &pRenderData->vertexBuffer, &pMesh->stride, &vertexByteOffset);

		if(pActiveMesh == nullptr)
		{
			deviceCtx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		}
	}
	else
	{
		// Set screen space quad
		deviceCtx->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);
		deviceCtx->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);

		deviceCtx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	}
}

void Direct3D11Render::UpdateDynamicMesh(DynamicMesh& dynamicMesh)
{
	MeshRenderDataD3D11* pRenderData = static_cast<MeshRenderDataD3D11*>(dynamicMesh.pRenderData);

	if(dynamicMesh.isIndicesDirty)
	{
		UpdateD3D11Resource(pRenderData->indexBuffer, dynamicMesh.indices.get(), dynamicMesh.GetSizeOfIndexData());
		dynamicMesh.isIndicesDirty = false;
	}

	if(dynamicMesh.isVerticesDirty)
	{
		UpdateD3D11Resource(pRenderData->vertexBuffer, dynamicMesh.vertices.get(), dynamicMesh.GetSizeOfVertexData());
		dynamicMesh.isVerticesDirty = false;
	}
}

void Direct3D11Render::SetVertexShader(Shader* vertexShader)
{
	if(pActiveVertexShader == vertexShader)
	{
		return;
	}

	const ShaderRenderDataD3D11* vertexShaderRenderData = static_cast<const ShaderRenderDataD3D11*>(vertexShader->renderData);

	deviceCtx->VSSetShader(vertexShaderRenderData->vertexShader, nullptr, 0);
	deviceCtx->VSSetConstantBuffers(0, (UINT)vertexShaderRenderData->constantBuffers.size(), vertexShaderRenderData->constantBuffers.data());

	pActiveVertexShader = vertexShader;
}

void Direct3D11Render::SetFragmentShader(Shader* pFragmentShader)
{
	if(pActiveFragmentShader == pFragmentShader)
	{
		return;
	}

	const ShaderRenderDataD3D11* pShaderRenderData = static_cast<const ShaderRenderDataD3D11*>(pFragmentShader->renderData);

	deviceCtx->PSSetShader(pShaderRenderData->pixelShader, nullptr, 0);
	deviceCtx->PSSetConstantBuffers(0, static_cast<UINT>(pShaderRenderData->constantBuffers.size()), pShaderRenderData->constantBuffers.data());

	pActiveFragmentShader = pFragmentShader;
}

void Direct3D11Render::SetTexture(uint8_t samplerIdx, Texture* texture, EFilterMode filterMode, EAddressMode addressMode)
{
	ASSERT(samplerIdx < D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT);

	TextureRenderDataD3D11* pRenderData = static_cast<TextureRenderDataD3D11*>(texture->renderData);

	deviceCtx->VSSetShaderResources(samplerIdx, 1, &pRenderData->textureSRV);
	deviceCtx->PSSetShaderResources(samplerIdx, 1, &pRenderData->textureSRV);

	static const D3D11_TEXTURE_ADDRESS_MODE addressModeConversionTable[] =
	{
		D3D11_TEXTURE_ADDRESS_WRAP,			// ETextureWrapMode::Repeat
		D3D11_TEXTURE_ADDRESS_CLAMP,		// ETextureWrapMode::Clamp
		D3D11_TEXTURE_ADDRESS_MIRROR,		// ETextureWrapMode::MirroredRepeat
		D3D11_TEXTURE_ADDRESS_MIRROR_ONCE,	// ETextureWrapMode::MirrorOnce
		D3D11_TEXTURE_ADDRESS_BORDER,		// ETextureWrapMode::Border
	};

	D3D11_SAMPLER_DESC samplerDesc = {};
	switch(filterMode)
	{
		case EFilterMode::Point:		samplerDesc.Filter = D3D11_ENCODE_BASIC_FILTER(D3D11_FILTER_TYPE_POINT, D3D11_FILTER_TYPE_POINT, D3D11_FILTER_TYPE_POINT, D3D11_FILTER_REDUCTION_TYPE_STANDARD); break;
		case EFilterMode::Bilinear:		samplerDesc.Filter = D3D11_ENCODE_BASIC_FILTER(D3D11_FILTER_TYPE_LINEAR, D3D11_FILTER_TYPE_LINEAR, D3D11_FILTER_TYPE_POINT, D3D11_FILTER_REDUCTION_TYPE_STANDARD); break;
		case EFilterMode::Trilinear:	samplerDesc.Filter = D3D11_ENCODE_BASIC_FILTER(D3D11_FILTER_TYPE_LINEAR, D3D11_FILTER_TYPE_LINEAR, D3D11_FILTER_TYPE_LINEAR, D3D11_FILTER_REDUCTION_TYPE_STANDARD); break;
		case EFilterMode::Anisotropic:	samplerDesc.Filter = D3D11_ENCODE_ANISOTROPIC_FILTER(D3D11_FILTER_REDUCTION_TYPE_STANDARD); break;
	};
	samplerDesc.AddressU = addressModeConversionTable[(size_t)addressMode];
	samplerDesc.AddressV = addressModeConversionTable[(size_t)addressMode];
	samplerDesc.AddressW = addressModeConversionTable[(size_t)addressMode];
	samplerDesc.MaxAnisotropy = D3D11_MAX_MAXANISOTROPY;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	SetSamplerState(samplerIdx, samplerDesc);
}

void Direct3D11Render::UpdateShaderParams(const Material* pMaterial)
{
	const ShaderRenderDataD3D11* pVertexShaderRenderData = static_cast<const ShaderRenderDataD3D11*>(pActiveVertexShader->renderData);
	UpdateShaderParams(pMaterial, pVertexShaderRenderData);

	const ShaderRenderDataD3D11* pFragmentShaderRenderData = static_cast<const ShaderRenderDataD3D11*>(pActiveFragmentShader->renderData);
	UpdateShaderParams(pMaterial, pFragmentShaderRenderData);
}

void Direct3D11Render::UpdateShaderParams(const Material* pMaterial, const ShaderRenderDataD3D11* pShaderRenderData)
{
	const std::pair<uint32_t, uint32_t>* pOffsetSizePair = nullptr;

	for(size_t i = 0; i < pShaderRenderData->constantBuffers.size(); ++i)
	{
		bool isChanged = false;
		const ShaderRenderDataD3D11::ConstantBufferData& bufferData = pShaderRenderData->constantBuffersData[i];

		for(const Material::ShaderParam& shaderParam : pMaterial->GetShaderParams())
		{
			pOffsetSizePair = bufferData.variableOffsetSizePairs.Find(shaderParam.name);
			if(pOffsetSizePair != nullptr)
			{
				isChanged |= CheckAndUpdateShaderParam(shaderParam.GetValue(), bufferData.data.ptr.get() + pOffsetSizePair->first, pOffsetSizePair->second);
			}
		}

		pOffsetSizePair = bufferData.variableOffsetSizePairs.Find("matWorldView");
		if(pOffsetSizePair != nullptr && pOffsetSizePair->second == sizeof(DirectX::XMMATRIX))
		{
			const DirectX::XMMATRIX matWorldView = DirectX::XMMatrixMultiply(matWorld, matView);
			isChanged |= CheckAndUpdateShaderParam(&matWorldView.r[0], bufferData.data.ptr.get() + pOffsetSizePair->first, pOffsetSizePair->second);
		}

		pOffsetSizePair = bufferData.variableOffsetSizePairs.Find("matWorldViewProj");
		if(pOffsetSizePair != nullptr && pOffsetSizePair->second == sizeof(DirectX::XMMATRIX))
		{
			const DirectX::XMMATRIX matWorldView = DirectX::XMMatrixMultiply(matWorld, matView);
			const DirectX::XMMATRIX matWorldViewProj = DirectX::XMMatrixMultiply(matWorldView, matProj);
			isChanged |= CheckAndUpdateShaderParam(&matWorldViewProj.r[0], bufferData.data.ptr.get() + pOffsetSizePair->first, pOffsetSizePair->second);
		}

		pOffsetSizePair = bufferData.variableOffsetSizePairs.Find("matView");
		if(pOffsetSizePair != nullptr)
		{
			isChanged |= CheckAndUpdateShaderParam(&matView.r[0], bufferData.data.ptr.get() + pOffsetSizePair->first, pOffsetSizePair->second);
		}

		pOffsetSizePair = bufferData.variableOffsetSizePairs.Find("matViewInv");
		if(pOffsetSizePair != nullptr)
		{
			const DirectX::XMMATRIX matViewInv = DirectX::XMMatrixInverse(nullptr, matView);
			isChanged |= CheckAndUpdateShaderParam(&matViewInv.r[0], bufferData.data.ptr.get() + pOffsetSizePair->first, pOffsetSizePair->second);
		}

		pOffsetSizePair = bufferData.variableOffsetSizePairs.Find("camPos");
		if(pOffsetSizePair != nullptr)
		{
			const DirectX::XMMATRIX matViewInv = DirectX::XMMatrixInverse(nullptr, matView);
			isChanged |= CheckAndUpdateShaderParam(&matViewInv.r[3], bufferData.data.ptr.get() + pOffsetSizePair->first, pOffsetSizePair->second);
		}

		pOffsetSizePair = bufferData.variableOffsetSizePairs.Find("resolution");
		if(pOffsetSizePair != nullptr && pOffsetSizePair->second == 2 * sizeof(float))
		{
			float resolution[2];
			if(pActiveView != nullptr)
			{
				resolution[0] = pActiveView->GetWidth();
				resolution[1] = pActiveView->GetHeight();
			}
			else
			{
				resolution[0] = pActiveWindow->GetWidth();
				resolution[1] = pActiveWindow->GetHeight();
			}

			isChanged |= CheckAndUpdateShaderParam(resolution, bufferData.data.ptr.get() + pOffsetSizePair->first, pOffsetSizePair->second);
		}

		if(isChanged)
		{
			deviceCtx->UpdateSubresource(pShaderRenderData->constantBuffers[i], 0, nullptr, bufferData.data.ptr.get(), 0, 0);
		}
	}
}

bool Direct3D11Render::CheckAndUpdateShaderParam(const void* value, void* valueInConstantBuffer, uint32_t size)
{
	if(memcmp(valueInConstantBuffer, value, size) == 0)
	{
		return false;
	}

	memcpy(valueInConstantBuffer, value, size);
	return true;
}

void Direct3D11Render::DoRender(uint32_t indexOffset, uint32_t vertexOffset, uint32_t numIndices, uint32_t numVertices)
{
	SetDepthStencilState();
	SetRasterizerState();
	SetBlendState();
	SetInputLayout();

	if(numIndices != 0)
	{
		deviceCtx->DrawIndexed(numIndices, indexOffset, vertexOffset);
	}
	else
	{
		deviceCtx->Draw(numVertices, vertexOffset);
	}
}

void Direct3D11Render::UpdateD3D11Resource(ID3D11Resource* pResource, const void* pData, size_t size)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr = deviceCtx->Map(pResource, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(hr))
	{
		LOG_WARNING("Failed to map D3D11 resource");
		return;
	}

	memcpy(mappedResource.pData, pData, size);
	deviceCtx->Unmap(pResource, 0);
}

void Direct3D11Render::SetDepthStencilState()
{
	ID3D11DepthStencilState* pDepthStencilState = nullptr;

	uint64_t key = 0;
	key |= (uint64_t)depthStencilDesc.DepthEnable					<< 0;	// 1 bit
	key |= (uint64_t)depthStencilDesc.DepthWriteMask				<< 1;	// 1 bit
	key |= (uint64_t)depthStencilDesc.DepthFunc						<< 2;	// 4 bits
	key |= (uint64_t)depthStencilDesc.StencilEnable					<< 6;	// 1 bit
	key |= (uint64_t)depthStencilDesc.StencilReadMask				<< 7;	// 8 bits
	key |= (uint64_t)depthStencilDesc.StencilWriteMask				<< 15;	// 8 bits
	key |= (uint64_t)depthStencilDesc.FrontFace.StencilFailOp		<< 23;	// 4 bits
	key |= (uint64_t)depthStencilDesc.FrontFace.StencilDepthFailOp	<< 27;	// 4 bits
	key |= (uint64_t)depthStencilDesc.FrontFace.StencilPassOp		<< 31;	// 4 bits
	key |= (uint64_t)depthStencilDesc.FrontFace.StencilFunc			<< 35;	// 4 bits
	key |= (uint64_t)depthStencilDesc.BackFace.StencilFailOp		<< 39;	// 4 bits
	key |= (uint64_t)depthStencilDesc.BackFace.StencilDepthFailOp	<< 43;	// 4 bits
	key |= (uint64_t)depthStencilDesc.BackFace.StencilPassOp		<< 47;	// 4 bits
	key |= (uint64_t)depthStencilDesc.BackFace.StencilFunc			<< 51;	// 4 bits

	auto it = depthStencilStateCache.find(key);
	if(it != depthStencilStateCache.end())
	{
		pDepthStencilState = it->second;
	}
	else
	{
		HRESULT hr = d3dDevice->CreateDepthStencilState(&depthStencilDesc, &pDepthStencilState);
		DX_CHECK_HRESULT(hr);
		depthStencilStateCache.insert(std::make_pair(key, pDepthStencilState));
	}

	if(pActiveDepthStencilState != pDepthStencilState)
	{
		deviceCtx->OMSetDepthStencilState(pDepthStencilState, 0);
		pActiveDepthStencilState = pDepthStencilState;
	}
}

void Direct3D11Render::SetRasterizerState()
{
	ID3D11RasterizerState* pRasterizerState = nullptr;

	uint64_t key = 0;
	key |= (uint64_t)rasterizerDesc.FillMode				<< 0;	// 2 bits
	key |= (uint64_t)rasterizerDesc.CullMode				<< 2;	// 2 bits
	key |= (uint64_t)rasterizerDesc.FrontCounterClockwise	<< 4;	// 1 bit
	key |= (uint64_t)rasterizerDesc.DepthBias				<< 5;	// 32 bits
//	key |= (uint64_t)rasterizerDesc.DepthBiasClamp			<< // Constant
//	key |= (uint64_t)rasterizerDesc.SlopeScaledDepthBias	<< // Constant
	key |= (uint64_t)rasterizerDesc.DepthClipEnable 		<< 37;	// 1 bit
	key |= (uint64_t)rasterizerDesc.ScissorEnable			<< 38;	// 1 bit
	key |= (uint64_t)rasterizerDesc.MultisampleEnable		<< 39;	// 1 bit
	key |= (uint64_t)rasterizerDesc.AntialiasedLineEnable	<< 40;	// 1 bit

	auto it = rasterizerStateCache.find(key);
	if(it != rasterizerStateCache.end())
	{
		pRasterizerState = it->second;
	}
	else
	{
		HRESULT hr = d3dDevice->CreateRasterizerState(&rasterizerDesc, &pRasterizerState);
		DX_CHECK_HRESULT(hr);
		rasterizerStateCache.insert(std::make_pair(key, pRasterizerState));
	}

	if(pActiveRasterizerState != pRasterizerState)
	{
		deviceCtx->RSSetState(pRasterizerState);
		pActiveRasterizerState = pRasterizerState;
	}
}

void Direct3D11Render::SetBlendState()
{
	ID3D11BlendState* pBlendState = nullptr;

	uint64_t key = 0;
	key |= (uint64_t)blendDesc.AlphaToCoverageEnable					<< 0;	// 1 bit
	key |= (uint64_t)blendDesc.IndependentBlendEnable					<< 1;	// 1 bit
	key |= (uint64_t)blendDesc.RenderTarget[0].BlendEnable				<< 2;	// 1 bit
	key |= (uint64_t)blendDesc.RenderTarget[0].SrcBlend					<< 3;	// 5 bits
	key |= (uint64_t)blendDesc.RenderTarget[0].DestBlend				<< 8;	// 5 bits
	key |= (uint64_t)blendDesc.RenderTarget[0].BlendOp					<< 13;	// 3 bits
	key |= (uint64_t)blendDesc.RenderTarget[0].SrcBlendAlpha			<< 16;	// 5 bits
	key |= (uint64_t)blendDesc.RenderTarget[0].DestBlendAlpha			<< 21;	// 5 bits
	key |= (uint64_t)blendDesc.RenderTarget[0].BlendOpAlpha				<< 26;	// 3 bits
	key |= (uint64_t)blendDesc.RenderTarget[0].RenderTargetWriteMask	<< 29;	// 4 bits [0, 15], see D3D11_COLOR_WRITE_ENABLE

	auto it = blendStateCache.find(key);
	if(it != blendStateCache.end())
	{
		pBlendState = it->second;
	}
	else
	{
		HRESULT hr = d3dDevice->CreateBlendState(&blendDesc, &pBlendState);
		DX_CHECK_HRESULT(hr);
		blendStateCache.insert(std::make_pair(key, pBlendState));
	}

	if(pActiveBlendState != pBlendState)
	{
		deviceCtx->OMSetBlendState(pBlendState, blendFactor, 0xffffffff);
		pActiveBlendState = pBlendState;
	}
}

void Direct3D11Render::SetInputLayout()
{
	ID3D11InputLayout* pInputLayout = nullptr;

	if(pActiveMesh != nullptr)
	{
		const MeshRenderDataD3D11* pMeshRenderData = static_cast<const MeshRenderDataD3D11*>(pActiveMesh->pRenderData);

		const std::pair<uint64_t, uint64_t> key = std::make_pair(pMeshRenderData->vertexLayoutKey, reinterpret_cast<uint64_t>(pActiveVertexShader->renderData));
		auto it = inputLayoutCache.find(key);
		if(it != inputLayoutCache.end())
		{
			pInputLayout = it->second;
		}
		else
		{
			static const D3D11_INPUT_ELEMENT_DESC s_attribConversionTable[] =
			{
				{ "POSITION",	0,	DXGI_FORMAT_R32G32B32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },		// Mesh::EAttrib::Position
				{ "NORMAL",		0,	DXGI_FORMAT_R32G32B32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },		// Mesh::EAttrib::Normal
				{ "COLOR",		0,	DXGI_FORMAT_R32G32B32A32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },		// Mesh::EAttrib::Color
				{ "TEXCOORD",	0,	DXGI_FORMAT_R32G32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },		// Mesh::EAttrib::Texcoord0
				{ "TEXCOORD",	1,	DXGI_FORMAT_R32G32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },		// Mesh::EAttrib::Texcoord1
				{ "TEXCOORD",	2,	DXGI_FORMAT_R32G32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },		// Mesh::EAttrib::Texcoord2
				{ "TEXCOORD",	3,	DXGI_FORMAT_R32G32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },		// Mesh::EAttrib::Texcoord3
				{ "TEXCOORD",	4,	DXGI_FORMAT_R32G32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },		// Mesh::EAttrib::Texcoord4
				{ "TEXCOORD",	5,	DXGI_FORMAT_R32G32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },		// Mesh::EAttrib::Texcoord5
				{ "TEXCOORD",	6,	DXGI_FORMAT_R32G32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },		// Mesh::EAttrib::Texcoord6
				{ "TEXCOORD",	7,	DXGI_FORMAT_R32G32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },		// Mesh::EAttrib::Texcoord7
			};
			DESIRE_CHECK_ARRAY_SIZE(s_attribConversionTable, Mesh::EAttrib::Num);

			static const DXGI_FORMAT s_attribTypeConversionTable[][4] =
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
			DESIRE_CHECK_ARRAY_SIZE(s_attribTypeConversionTable, Mesh::EAttribType::Num);

			const Array<Mesh::VertexLayout>& vertexLayout = pActiveMesh->GetVertexLayout();
			D3D11_INPUT_ELEMENT_DESC vertexElementDesc[static_cast<size_t>(Mesh::EAttrib::Num)] = {};
			for(size_t i = 0; i < vertexLayout.Size(); ++i)
			{
				const Mesh::VertexLayout& layout = vertexLayout[i];
				vertexElementDesc[i] = s_attribConversionTable[static_cast<size_t>(layout.attrib)];
				vertexElementDesc[i].Format = s_attribTypeConversionTable[static_cast<size_t>(layout.type)][layout.count - 1];
			}

			const ShaderRenderDataD3D11* pVertexShaderRenderData = static_cast<const ShaderRenderDataD3D11*>(pActiveVertexShader->renderData);
			HRESULT hr = d3dDevice->CreateInputLayout(vertexElementDesc, static_cast<UINT>(vertexLayout.Size()), pVertexShaderRenderData->shaderCode->GetBufferPointer(), pVertexShaderRenderData->shaderCode->GetBufferSize(), &pInputLayout);
			DX_CHECK_HRESULT(hr);
			inputLayoutCache.emplace(key, pInputLayout);
		}
	}

	if(pActiveInputLayout != pInputLayout)
	{
		deviceCtx->IASetInputLayout(pInputLayout);
		pActiveInputLayout = pInputLayout;
	}
}

void Direct3D11Render::SetSamplerState(uint8_t samplerIdx, const D3D11_SAMPLER_DESC& samplerDesc)
{
	ID3D11SamplerState* samplerState = nullptr;

	uint64_t key = 0;
	key |= (uint64_t)samplerDesc.Filter			<< 0;	// 9 bits
	key |= (uint64_t)samplerDesc.AddressU		<< 9;	// 3 bits
	key |= (uint64_t)samplerDesc.AddressV		<< 12;	// 3 bits
	key |= (uint64_t)samplerDesc.AddressW		<< 15;	// 3 bits
//	key |= (uint64_t)samplerDesc.MipLODBias		<< // Constant
	key |= (uint64_t)samplerDesc.MaxAnisotropy	<< 18;	// 5 bits [1, 16]
	key |= (uint64_t)samplerDesc.ComparisonFunc	<< 23;	// 4 bits
//	key |= (uint64_t)samplerDesc.BorderColor	<< // Constant
//	key |= (uint64_t)samplerDesc.MinLOD			<< // Constant
//	key |= (uint64_t)samplerDesc.MaxLOD			<< // Constant

	auto it = samplerStateCache.find(key);
	if(it != samplerStateCache.end())
	{
		samplerState = it->second;
	}
	else
	{
		HRESULT hr = d3dDevice->CreateSamplerState(&samplerDesc, &samplerState);
		DX_CHECK_HRESULT(hr);
		samplerStateCache.insert(std::make_pair(key, samplerState));
	}

	if(activeSamplerStates[samplerIdx] != samplerState)
	{
		deviceCtx->VSSetSamplers(samplerIdx, 1, &samplerState);
		deviceCtx->PSSetSamplers(samplerIdx, 1, &samplerState);
		activeSamplerStates[samplerIdx] = samplerState;
	}
}

DXGI_FORMAT Direct3D11Render::GetTextureFormat(const Texture* pTexture)
{
	const DXGI_FORMAT conversionTable[] =
	{
		DXGI_FORMAT_UNKNOWN,					// Texture::EFormat::Unknown
		DXGI_FORMAT_R8_UNORM,					// Texture::EFormat::R8
		DXGI_FORMAT_R8G8_UNORM,					// Texture::EFormat::RG8
		DXGI_FORMAT_R8G8B8A8_UNORM,				// Texture::EFormat::RGB8
		DXGI_FORMAT_R8G8B8A8_UNORM,				// Texture::EFormat::RGBA8
		DXGI_FORMAT_R32G32B32_FLOAT,			// Texture::EFormat::RGB32F
		DXGI_FORMAT_R32G32B32A32_FLOAT,			// Texture::EFormat::RGBA32F
		DXGI_FORMAT_D16_UNORM,					// Texture::EFormat::D16
		DXGI_FORMAT_D24_UNORM_S8_UINT,			// Texture::EFormat::D24S8
		DXGI_FORMAT_D32_FLOAT,					// Texture::EFormat::D32
	};
	DESIRE_CHECK_ARRAY_SIZE(conversionTable, Texture::EFormat::D32 + 1);

	return conversionTable[static_cast<size_t>(pTexture->format)];
}
