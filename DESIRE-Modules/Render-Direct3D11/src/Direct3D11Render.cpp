#include "stdafx_Direct3D11.h"
#include "Direct3D11Render.h"

#include "DirectXMathExt.h"
#include "MeshRenderDataD3D11.h"
#include "RenderableRenderDataD3D11.h"
#include "RenderTargetRenderDataD3D11.h"
#include "ShaderRenderDataD3D11.h"
#include "TextureRenderDataD3D11.h"

#include "Engine/Application/OSWindow.h"

#include "Engine/Core/FS/FileSystem.h"
#include "Engine/Core/Math/Matrix4.h"
#include "Engine/Core/String/StackString.h"

#include "Engine/Render/Material.h"
#include "Engine/Render/Mesh.h"
#include "Engine/Render/Renderable.h"
#include "Engine/Render/RenderTarget.h"
#include "Engine/Render/Shader.h"
#include "Engine/Render/Texture.h"

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

static DXGI_FORMAT GetTextureFormat(const Texture& texture)
{
	const DXGI_FORMAT conversionTable[] =
	{
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

	return conversionTable[static_cast<size_t>(texture.GetFormat())];
}

Direct3D11Render::Direct3D11Render()
{
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
	m_errorVertexShader = std::make_unique<Shader>("vs_error");
	m_errorVertexShader->m_data = MemoryBuffer::CreateFromDataCopy(vs_error, sizeof(vs_error));
	m_errorPixelShader = std::make_unique<Shader>("ps_error");
	m_errorPixelShader->m_data = MemoryBuffer::CreateFromDataCopy(ps_error, sizeof(ps_error));

	m_depthStencilDesc = CD3D11_DEPTH_STENCIL_DESC(D3D11_DEFAULT);

	m_rasterizerDesc = CD3D11_RASTERIZER_DESC(D3D11_DEFAULT);
	m_rasterizerDesc.DepthClipEnable = FALSE;
	m_rasterizerDesc.MultisampleEnable = TRUE;
	m_rasterizerDesc.AntialiasedLineEnable = TRUE;

	m_blendDesc.RenderTarget[0].BlendEnable = TRUE;
	m_blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	m_blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	m_blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	m_blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
	m_blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
	m_blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	m_blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	m_matWorld = DirectX::XMMatrixIdentity();
	m_matView = DirectX::XMMatrixIdentity();
	m_matProj = DirectX::XMMatrixIdentity();
}

Direct3D11Render::~Direct3D11Render()
{
}

bool Direct3D11Render::Init(OSWindow& mainWindow)
{
	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChainDesc.BufferDesc.Width = mainWindow.GetWidth();
	swapChainDesc.BufferDesc.Height = mainWindow.GetHeight();
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 1;
	swapChainDesc.OutputWindow = static_cast<HWND>(mainWindow.GetHandle());
	swapChainDesc.Windowed = TRUE;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	const D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1
	};

	HRESULT hr = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
#if defined(_DEBUG)
		D3D11_CREATE_DEVICE_DEBUG,
#else
		D3D11_CREATE_DEVICE_SINGLETHREADED,
#endif
		featureLevels,
		static_cast<UINT>(DESIRE_ASIZEOF(featureLevels)),
		D3D11_SDK_VERSION,
		&swapChainDesc,
		&m_pSwapChain,
		&m_pDevice,
		nullptr,
		&m_pDeviceCtx);

	if(FAILED(hr))
	{
		return false;
	}

	bool succeeded = CreateFrameBuffers(mainWindow.GetWidth(), mainWindow.GetHeight());
	if(!succeeded)
	{
		return false;
	}

	m_pDeviceCtx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	SetDefaultRenderStates();

	return true;
}

void Direct3D11Render::UpdateRenderWindow(OSWindow& window)
{
	ID3D11RenderTargetView* nullViews[] = { nullptr };
	m_pDeviceCtx->OMSetRenderTargets(1, nullViews, nullptr);
	DX_RELEASE(m_pFrameBufferDSV);
	DX_RELEASE(m_pFrameBufferRTV);
	m_pDeviceCtx->Flush();

	HRESULT hr = m_pSwapChain->ResizeBuffers(0, window.GetWidth(), window.GetHeight(), DXGI_FORMAT_UNKNOWN, 0);
	if(hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
	{
		// Have to destroy the device, swapchain, and all resources and recreate them to recover from this case
	}

	DX_CHECK_HRESULT(hr);

	bool succeeded = CreateFrameBuffers(window.GetWidth(), window.GetHeight());
	ASSERT(succeeded);
}

void Direct3D11Render::Kill()
{
	Unbind(*m_errorVertexShader);
	Unbind(*m_errorPixelShader);

	m_pActiveWindow = nullptr;
	m_pActiveMesh = nullptr;
	m_pActiveRenderTarget = nullptr;

	m_pActiveVS = nullptr;
	m_pActivePS = nullptr;
	m_pActiveDepthStencilState = nullptr;
	m_pActiveRasterizerState = nullptr;
	m_pActiveBlendState = nullptr;
	m_pActiveInputLayout = nullptr;

	for(auto& pair : m_depthStencilStateCache)
	{
		DX_RELEASE(pair.second);
	}
	m_depthStencilStateCache.clear();

	for(auto& pair : m_rasterizerStateCache)
	{
		DX_RELEASE(pair.second);
	}
	m_rasterizerStateCache.clear();

	for(auto& pair : m_blendStateCache)
	{
		DX_RELEASE(pair.second);
	}
	m_blendStateCache.clear();

	m_inputLayoutCache.clear();

	for(auto& pair : m_samplerStateCache)
	{
		DX_RELEASE(pair.second);
	}
	m_samplerStateCache.clear();
	memset(m_activeSamplerStates, 0, sizeof(m_activeSamplerStates));

	DX_RELEASE(m_pFrameBufferDSV);
	DX_RELEASE(m_pFrameBufferRTV);
	DX_RELEASE(m_pSwapChain);
	DX_RELEASE(m_pDeviceCtx);
	DX_RELEASE(m_pDevice);
}

void Direct3D11Render::AppendShaderFilenameWithPath(WritableString& outString, const String& shaderFilename) const
{
	outString += "data/shaders/hlsl/";
	outString += shaderFilename;
	outString += ".hlsl";
}

void Direct3D11Render::EndFrame()
{
	HRESULT hr = m_pSwapChain->Present(1, 0);
	DX_CHECK_HRESULT(hr);
}

void Direct3D11Render::Clear(uint32_t clearColorRGBA, float depth, uint8_t stencil)
{
	const float clearColor[4] =
	{
		((clearColorRGBA >> 24) & 0xFF) / 255.0f,
		((clearColorRGBA >> 16) & 0xFF) / 255.0f,
		((clearColorRGBA >>  8) & 0xFF) / 255.0f,
		((clearColorRGBA >>  0) & 0xFF) / 255.0f,
	};

	if(m_pActiveRenderTarget != nullptr)
	{
		RenderTargetRenderDataD3D11* pRenderTargetRenderData = static_cast<RenderTargetRenderDataD3D11*>(m_pActiveRenderTarget->m_pRenderData);
		if(pRenderTargetRenderData != nullptr)
		{
			for(uint32_t i = 0; i < pRenderTargetRenderData->m_numRTVs; ++i)
			{
				m_pDeviceCtx->ClearRenderTargetView(pRenderTargetRenderData->m_RTVs[i], clearColor);
			}

			if(pRenderTargetRenderData->m_pDSV != nullptr)
			{
				m_pDeviceCtx->ClearDepthStencilView(pRenderTargetRenderData->m_pDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, depth, stencil);
			}
		}
	}
	else
	{
		m_pDeviceCtx->ClearRenderTargetView(m_pFrameBufferRTV, clearColor);
		m_pDeviceCtx->ClearDepthStencilView(m_pFrameBufferDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, depth, stencil);
	}
}

void Direct3D11Render::SetWorldMatrix(const Matrix4& matrix)
{
	m_matWorld.r[0] = GetXMVECTOR(matrix.col0);
	m_matWorld.r[1] = GetXMVECTOR(matrix.col1);
	m_matWorld.r[2] = GetXMVECTOR(matrix.col2);
	m_matWorld.r[3] = GetXMVECTOR(matrix.col3);
}

void Direct3D11Render::SetViewProjectionMatrices(const Matrix4& viewMatrix, const Matrix4& projMatrix)
{
	m_matView.r[0] = GetXMVECTOR(viewMatrix.col0);
	m_matView.r[1] = GetXMVECTOR(viewMatrix.col1);
	m_matView.r[2] = GetXMVECTOR(viewMatrix.col2);
	m_matView.r[3] = GetXMVECTOR(viewMatrix.col3);

	m_matProj.r[0] = GetXMVECTOR(projMatrix.col0);
	m_matProj.r[1] = GetXMVECTOR(projMatrix.col1);
	m_matProj.r[2] = GetXMVECTOR(projMatrix.col2);
	m_matProj.r[3] = GetXMVECTOR(projMatrix.col3);
}

void Direct3D11Render::SetScissor(uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
	if(x == 0 && y == 0 && width == 0 && height == 0)
	{
		m_rasterizerDesc.ScissorEnable = FALSE;
	}
	else
	{
		m_rasterizerDesc.ScissorEnable = TRUE;

		const D3D11_RECT rect = { x, y, x + width, y + height };
		m_pDeviceCtx->RSSetScissorRects(1, &rect);
	}
}

void Direct3D11Render::SetColorWriteEnabled(bool r, bool g, bool b, bool a)
{
	ASSERT(!m_blendDesc.IndependentBlendEnable && "Independent render target blend states are not supported (only the RenderTarget[0] members are used)");

	if(r)
	{
		m_blendDesc.RenderTarget[0].RenderTargetWriteMask |= D3D11_COLOR_WRITE_ENABLE_RED;
	}
	else
	{
		m_blendDesc.RenderTarget[0].RenderTargetWriteMask &= ~D3D11_COLOR_WRITE_ENABLE_RED;
	}

	if(g)
	{
		m_blendDesc.RenderTarget[0].RenderTargetWriteMask |= D3D11_COLOR_WRITE_ENABLE_GREEN;
	}
	else
	{
		m_blendDesc.RenderTarget[0].RenderTargetWriteMask &= ~D3D11_COLOR_WRITE_ENABLE_GREEN;
	}

	if(b)
	{
		m_blendDesc.RenderTarget[0].RenderTargetWriteMask |= D3D11_COLOR_WRITE_ENABLE_BLUE;
	}
	else
	{
		m_blendDesc.RenderTarget[0].RenderTargetWriteMask &= ~D3D11_COLOR_WRITE_ENABLE_BLUE;
	}

	if(a)
	{
		m_blendDesc.RenderTarget[0].RenderTargetWriteMask |= D3D11_COLOR_WRITE_ENABLE_ALPHA;
	}
	else
	{
		m_blendDesc.RenderTarget[0].RenderTargetWriteMask &= ~D3D11_COLOR_WRITE_ENABLE_ALPHA;
	}
}

void Direct3D11Render::SetDepthWriteEnabled(bool enabled)
{
	m_depthStencilDesc.DepthWriteMask = enabled ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
}

void Direct3D11Render::SetDepthTest(EDepthTest depthTest)
{
	switch(depthTest)
	{
		case EDepthTest::Disabled:
			m_depthStencilDesc.DepthEnable = FALSE;
			return;

		case EDepthTest::Less:			m_depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS; break;
		case EDepthTest::LessEqual:		m_depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL; break;
		case EDepthTest::Greater:		m_depthStencilDesc.DepthFunc = D3D11_COMPARISON_GREATER; break;
		case EDepthTest::GreaterEqual:	m_depthStencilDesc.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL; break;
		case EDepthTest::Equal:			m_depthStencilDesc.DepthFunc = D3D11_COMPARISON_EQUAL; break;
		case EDepthTest::NotEqual:		m_depthStencilDesc.DepthFunc = D3D11_COMPARISON_NOT_EQUAL; break;
	}

	m_depthStencilDesc.DepthEnable = TRUE;
}

void Direct3D11Render::SetCullMode(ECullMode cullMode)
{
	switch(cullMode)
	{
		case ECullMode::None:	m_rasterizerDesc.CullMode = D3D11_CULL_NONE; break;
		case ECullMode::CCW:	m_rasterizerDesc.CullMode = D3D11_CULL_BACK; break;
		case ECullMode::CW:		m_rasterizerDesc.CullMode = D3D11_CULL_FRONT; break;
	}
}

void Direct3D11Render::SetBlendModeSeparated(EBlend srcBlendRGB, EBlend destBlendRGB, EBlendOp blendOpRGB, EBlend srcBlendAlpha, EBlend destBlendAlpha, EBlendOp blendOpAlpha)
{
	ASSERT(!m_blendDesc.IndependentBlendEnable && "Independent render target blend states are not supported (only the RenderTarget[0] members are used)");

	m_blendDesc.RenderTarget[0].BlendEnable = TRUE;

	static constexpr D3D11_BLEND s_blendConversionTable[] =
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
	DESIRE_CHECK_ARRAY_SIZE(s_blendConversionTable, EBlend::InvBlendFactor + 1);

	m_blendDesc.RenderTarget[0].SrcBlend = s_blendConversionTable[(size_t)srcBlendRGB];
	m_blendDesc.RenderTarget[0].DestBlend = s_blendConversionTable[(size_t)destBlendRGB];
	m_blendDesc.RenderTarget[0].SrcBlendAlpha = s_blendConversionTable[(size_t)srcBlendAlpha];
	m_blendDesc.RenderTarget[0].DestBlendAlpha = s_blendConversionTable[(size_t)destBlendAlpha];

	static constexpr D3D11_BLEND_OP s_equationConversionTable[] =
	{
		D3D11_BLEND_OP_ADD,				// EBlendOp::Add
		D3D11_BLEND_OP_SUBTRACT,		// EBlendOp::Subtract
		D3D11_BLEND_OP_REV_SUBTRACT,	// EBlendOp::RevSubtract
		D3D11_BLEND_OP_MIN,				// EBlendOp::Min
		D3D11_BLEND_OP_MAX				// EBlendOp::Max
	};
	DESIRE_CHECK_ARRAY_SIZE(s_equationConversionTable, EBlendOp::Max + 1);

	m_blendDesc.RenderTarget[0].BlendOp = s_equationConversionTable[(size_t)blendOpRGB];
	m_blendDesc.RenderTarget[0].BlendOpAlpha = s_equationConversionTable[(size_t)blendOpAlpha];
}

void Direct3D11Render::SetBlendModeDisabled()
{
	m_blendDesc.RenderTarget[0].BlendEnable = FALSE;
}

void* Direct3D11Render::CreateRenderableRenderData(const Renderable& renderable)
{
	RenderableRenderDataD3D11* pRenderableRenderData = new RenderableRenderDataD3D11();

	static constexpr D3D11_INPUT_ELEMENT_DESC s_attribConversionTable[] =
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

	static constexpr DXGI_FORMAT s_attribTypeConversionTable[][4] =
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

	const MeshRenderDataD3D11* pMeshRenderData = static_cast<const MeshRenderDataD3D11*>(renderable.m_mesh->m_pRenderData);
	const ShaderRenderDataD3D11* pVS = static_cast<const ShaderRenderDataD3D11*>(renderable.m_material->m_vertexShader->m_pRenderData);
	pRenderableRenderData->m_inputLayoutKey = std::pair(pMeshRenderData->m_vertexLayoutKey, reinterpret_cast<uint64_t>(pVS));
	auto it = m_inputLayoutCache.find(pRenderableRenderData->m_inputLayoutKey);
	if(it != m_inputLayoutCache.end())
	{
		pRenderableRenderData->m_pInputLayout = it->second;
		pRenderableRenderData->m_pInputLayout->AddRef();
	}
	else
	{
		const Array<Mesh::VertexLayout>& vertexLayout = renderable.m_mesh->GetVertexLayout();
		D3D11_INPUT_ELEMENT_DESC vertexElementDesc[static_cast<size_t>(Mesh::EAttrib::Num)] = {};
		for(size_t i = 0; i < vertexLayout.Size(); ++i)
		{
			const Mesh::VertexLayout& layout = vertexLayout[i];
			vertexElementDesc[i] = s_attribConversionTable[static_cast<size_t>(layout.m_attrib)];
			vertexElementDesc[i].Format = s_attribTypeConversionTable[static_cast<size_t>(layout.m_type)][layout.m_count - 1];
		}

		HRESULT hr = m_pDevice->CreateInputLayout(vertexElementDesc, static_cast<UINT>(vertexLayout.Size()), pVS->m_pShaderCode->GetBufferPointer(), pVS->m_pShaderCode->GetBufferSize(), &pRenderableRenderData->m_pInputLayout);
		DX_CHECK_HRESULT(hr);

		m_inputLayoutCache.emplace(pRenderableRenderData->m_inputLayoutKey, pRenderableRenderData->m_pInputLayout);
	}

	return pRenderableRenderData;
}

void* Direct3D11Render::CreateMeshRenderData(const Mesh& mesh)
{
	MeshRenderDataD3D11* pMeshRenderData = new MeshRenderDataD3D11();

	const Array<Mesh::VertexLayout>& vertexLayout = mesh.GetVertexLayout();
	ASSERT(vertexLayout.Size() <= 9 && "It is possible to encode maximum of 9 vertex layouts into 64-bit");
	for(size_t i = 0; i < vertexLayout.Size(); ++i)
	{
		const Mesh::VertexLayout& layout = vertexLayout[i];
		pMeshRenderData->m_vertexLayoutKey |= (uint64_t)layout.m_attrib			<< (i * 7 + 0);	// 4 bits
		pMeshRenderData->m_vertexLayoutKey |= (uint64_t)layout.m_type			<< (i * 7 + 4);	// 1 bit
		pMeshRenderData->m_vertexLayoutKey |= (uint64_t)(layout.m_count - 1)	<< (i * 7 + 5);	// 2 bits [0, 3]
	}

	D3D11_BUFFER_DESC bufferDesc = {};
	switch(mesh.GetType())
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

	if(mesh.GetNumIndices() != 0)
	{
		bufferDesc.ByteWidth = mesh.GetSizeOfIndexData();
		bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA indexData = {};
		indexData.pSysMem = mesh.m_indices.get();

		HRESULT hr = m_pDevice->CreateBuffer(&bufferDesc, &indexData, &pMeshRenderData->m_pIndexBuffer);
		DX_CHECK_HRESULT(hr);
	}

	bufferDesc.ByteWidth = mesh.GetSizeOfVertexData();
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA vertexData = {};
	vertexData.pSysMem = mesh.m_vertices.get();

	HRESULT hr = m_pDevice->CreateBuffer(&bufferDesc, &vertexData, &pMeshRenderData->m_pVertexBuffer);
	DX_CHECK_HRESULT(hr);

	return pMeshRenderData;
}

void* Direct3D11Render::CreateShaderRenderData(const Shader& shader)
{
	ShaderRenderDataD3D11* pShaderRenderData = new ShaderRenderDataD3D11();

	D3D_SHADER_MACRO defines[32] = {};
	ASSERT(shader.m_defines.size() < DESIRE_ASIZEOF(defines));
	D3D_SHADER_MACRO* definePtr = &defines[0];
	for(const String& define : shader.m_defines)
	{
		definePtr->Name = define.Str();
		definePtr->Definition = "1";
		definePtr++;
	}

	StackString<DESIRE_MAX_PATH_LEN> filenameWithPath = FileSystem::Get()->GetAppDirectory();
	AppendShaderFilenameWithPath(filenameWithPath, shader.m_name);

	const bool isVertexShader = shader.m_name.StartsWith("vs_");
	UINT compileFlags = 0;
/**/compileFlags = D3DCOMPILE_SKIP_OPTIMIZATION;

	ID3DBlob* pErrorBlob = nullptr;
	HRESULT hr = D3DCompile(shader.m_data.ptr.get(),	// pSrcData
		shader.m_data.size,								// SrcDataSize
		filenameWithPath.Str(),							// pSourceName
		defines,										// pDefines
		D3D_COMPILE_STANDARD_FILE_INCLUDE,				// pInclude
		"main",											// pEntrypoint
		isVertexShader ? "vs_5_0" : "ps_5_0",			// pTarget
		compileFlags,									// D3DCOMPILE flags
		0,												// D3DCOMPILE_EFFECT flags
		&pShaderRenderData->m_pShaderCode,				// ppCode
		&pErrorBlob);									// ppErrorMsgs
	if(FAILED(hr))
	{
		if(pErrorBlob != nullptr)
		{
			LOG_ERROR("Shader compile error:\n%s", static_cast<const char*>(pErrorBlob->GetBufferPointer()));
			DX_RELEASE(pErrorBlob);
		}

		delete pShaderRenderData;
		return isVertexShader ? m_errorVertexShader->m_pRenderData : m_errorPixelShader->m_pRenderData;
	}

	if(isVertexShader)
	{
		hr = m_pDevice->CreateVertexShader(pShaderRenderData->m_pShaderCode->GetBufferPointer(), pShaderRenderData->m_pShaderCode->GetBufferSize(), nullptr, &pShaderRenderData->m_pVertexShader);
	}
	else
	{
		hr = m_pDevice->CreatePixelShader(pShaderRenderData->m_pShaderCode->GetBufferPointer(), pShaderRenderData->m_pShaderCode->GetBufferSize(), nullptr, &pShaderRenderData->m_pPixelShader);
	}
	DX_CHECK_HRESULT(hr);

	pShaderRenderData->m_pPtr->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(shader.m_name.Length()), shader.m_name.Str());

	ID3D11ShaderReflection* pReflection = nullptr;
	hr = D3DReflect(pShaderRenderData->m_pShaderCode->GetBufferPointer(), pShaderRenderData->m_pShaderCode->GetBufferSize(), IID_PPV_ARGS(&pReflection));
	if(FAILED(hr))
	{
		LOG_ERROR("D3DReflect failed 0x%08x\n", static_cast<uint32_t>(hr));
	}

	D3D11_SHADER_DESC shaderDesc;
	hr = pReflection->GetDesc(&shaderDesc);
	if(FAILED(hr))
	{
		LOG_ERROR("ID3D11ShaderReflection::GetDesc failed 0x%08x\n", (uint32_t)hr);
	}

	pShaderRenderData->m_constantBuffers.SetSize(shaderDesc.ConstantBuffers);
	pShaderRenderData->m_constantBuffersData.SetSize(shaderDesc.ConstantBuffers);
	for(uint32_t i = 0; i < shaderDesc.ConstantBuffers; ++i)
	{
		ID3D11ShaderReflectionConstantBuffer* pConstantBuffer = pReflection->GetConstantBufferByIndex(i);
		D3D11_SHADER_BUFFER_DESC shaderBufferDesc;
		hr = pConstantBuffer->GetDesc(&shaderBufferDesc);
		DX_CHECK_HRESULT(hr);

		if(shaderBufferDesc.Type == D3D_CT_CBUFFER || shaderBufferDesc.Type == D3D_CT_TBUFFER)
		{
			// Create constant buffer
			D3D11_BUFFER_DESC bufferDesc = {};
			bufferDesc.ByteWidth = shaderBufferDesc.Size;
			bufferDesc.Usage = D3D11_USAGE_DEFAULT;
			bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

			hr = m_pDevice->CreateBuffer(&bufferDesc, nullptr, &pShaderRenderData->m_constantBuffers[i]);
			DX_CHECK_HRESULT(hr);

			// Create constant buffer data
			ShaderRenderDataD3D11::ConstantBufferData& bufferData = pShaderRenderData->m_constantBuffersData[i];
			bufferData.m_data = MemoryBuffer(shaderBufferDesc.Size);

			for(uint32_t j = 0; j < shaderBufferDesc.Variables; ++j)
			{
				ID3D11ShaderReflectionVariable* pVariable = pConstantBuffer->GetVariableByIndex(j);
				D3D11_SHADER_VARIABLE_DESC varDesc;
				hr = pVariable->GetDesc(&varDesc);
				DX_CHECK_HRESULT(hr);

				if((varDesc.uFlags & D3D_SVF_USED) == 0)
				{
					continue;
				}

				ID3D11ShaderReflectionType* pType = pVariable->GetType();
				D3D11_SHADER_TYPE_DESC typeDesc;
				hr = pType->GetDesc(&typeDesc);
				DX_CHECK_HRESULT(hr);

				if( typeDesc.Class == D3D_SVC_SCALAR ||
					typeDesc.Class == D3D_SVC_VECTOR ||
					typeDesc.Class == D3D_SVC_MATRIX_ROWS || typeDesc.Class == D3D_SVC_MATRIX_COLUMNS)
				{
					const HashedString key = HashedString::CreateFromString(String(varDesc.Name, strlen(varDesc.Name)));
					bufferData.m_variables.Insert(key, { bufferData.m_data.ptr.get() + varDesc.StartOffset, varDesc.Size });
				}
			}
		}
	}

	DX_RELEASE(pReflection);

	return pShaderRenderData;
}

void* Direct3D11Render::CreateTextureRenderData(const Texture& texture)
{
	TextureRenderDataD3D11* pTextureRenderData = new TextureRenderDataD3D11();

	const bool isRenderTarget = (texture.GetData() == nullptr);

	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = texture.GetWidth();
	textureDesc.Height = texture.GetHeight();
	textureDesc.MipLevels = texture.GetNumMipLevels();
	textureDesc.ArraySize = 1;
	textureDesc.Format = GetTextureFormat(texture);
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	if(texture.IsDepthFormat())
	{
		textureDesc.BindFlags |= D3D11_BIND_DEPTH_STENCIL;
	}
	else if(isRenderTarget)
	{
		textureDesc.BindFlags |= D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		if(texture.GetNumMipLevels() > 1)
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
		HRESULT hr = m_pDevice->CreateTexture2D(&textureDesc, nullptr, &pTextureRenderData->m_pTexture2D);
		DX_CHECK_HRESULT(hr);
	}
	else
	{
		const uint32_t numTextureData = textureDesc.MipLevels * textureDesc.ArraySize;
		DESIRE_STACKALLOCATE_ARRAY(D3D11_SUBRESOURCE_DATA, textureData, numTextureData);
		ASSERT(numTextureData == 1 && "TODO: Set pSysMem properly in the loop below");
		for(uint32_t i = 0; i < numTextureData; ++i)
		{
			textureData[i].pSysMem = texture.GetData();
			textureData[i].SysMemPitch = texture.GetWidth() * texture.GetBytesPerPixel();
			textureData[i].SysMemSlicePitch = textureData[i].SysMemPitch * texture.GetHeight();
		}

		HRESULT hr = m_pDevice->CreateTexture2D(&textureDesc, textureData, &pTextureRenderData->m_pTexture2D);
		DX_CHECK_HRESULT(hr);
	}

	// Create SRV
	if(textureDesc.BindFlags & D3D11_BIND_SHADER_RESOURCE)
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = textureDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = textureDesc.MipLevels;

		HRESULT hr = m_pDevice->CreateShaderResourceView(pTextureRenderData->m_pTexture, &srvDesc, &pTextureRenderData->m_pSRV);
		DX_CHECK_HRESULT(hr);
	}

	return pTextureRenderData;
}

void* Direct3D11Render::CreateRenderTargetRenderData(const RenderTarget& renderTarget)
{
	RenderTargetRenderDataD3D11* pRenderTargetRenderData = new RenderTargetRenderDataD3D11();

	const uint8_t textureCount = std::min<uint8_t>(renderTarget.GetTextureCount(), D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT);
	for(uint8_t i = 0; i < textureCount; ++i)
	{
		const std::shared_ptr<Texture>& texture = renderTarget.GetTexture(i);
		const TextureRenderDataD3D11* pTextureRenderData = static_cast<const TextureRenderDataD3D11*>(texture->m_pRenderData);
		ASSERT(pTextureRenderData != nullptr);

		if(texture->IsDepthFormat())
		{
			ASSERT(pRenderTargetRenderData->m_pDSV == nullptr && "RenderTargets can have only 1 depth attachment");

			HRESULT hr = m_pDevice->CreateDepthStencilView(pTextureRenderData->m_pTexture, nullptr, &pRenderTargetRenderData->m_pDSV);
			DX_CHECK_HRESULT(hr);
		}
		else
		{
			ID3D11RenderTargetView* pRTV = nullptr;
			HRESULT hr = m_pDevice->CreateRenderTargetView(pTextureRenderData->m_pTexture, nullptr, &pRTV);
			DX_CHECK_HRESULT(hr);

			pRenderTargetRenderData->m_RTVs[pRenderTargetRenderData->m_numRTVs++] = pRTV;
		}
	}

	return pRenderTargetRenderData;
}

void Direct3D11Render::DestroyRenderableRenderData(void* pRenderData)
{
	RenderableRenderDataD3D11* pRenderableRenderData = static_cast<RenderableRenderDataD3D11*>(pRenderData);

	if(m_pActiveInputLayout == pRenderableRenderData->m_pInputLayout)
	{
		m_pActiveInputLayout = nullptr;
	}

	UINT refCount = pRenderableRenderData->m_pInputLayout->Release();
	if(refCount == 0)
	{
		m_inputLayoutCache.erase(pRenderableRenderData->m_inputLayoutKey);
	}

	delete pRenderableRenderData;
}

void Direct3D11Render::DestroyMeshRenderData(void* pRenderData)
{
	MeshRenderDataD3D11* pMeshRenderData = static_cast<MeshRenderDataD3D11*>(pRenderData);

	DX_RELEASE(pMeshRenderData->m_pIndexBuffer);
	DX_RELEASE(pMeshRenderData->m_pVertexBuffer);

	delete pMeshRenderData;
}

void Direct3D11Render::DestroyShaderRenderData(void* pRenderData)
{
	ShaderRenderDataD3D11* pShaderRenderData = static_cast<ShaderRenderDataD3D11*>(pRenderData);

	if(pShaderRenderData == m_errorVertexShader->m_pRenderData || pShaderRenderData == m_errorPixelShader->m_pRenderData)
	{
		return;
	}

	if(m_pActiveVS == pRenderData)
	{
		m_pActiveVS = nullptr;
	}

	if(m_pActivePS == pRenderData)
	{
		m_pActivePS = nullptr;
	}

	DX_RELEASE(pShaderRenderData->m_pPtr);
	DX_RELEASE(pShaderRenderData->m_pShaderCode);

	for(ID3D11Buffer* pBuffer : pShaderRenderData->m_constantBuffers)
	{
		DX_RELEASE(pBuffer);
	}
	pShaderRenderData->m_constantBuffers.Clear();
	pShaderRenderData->m_constantBuffersData.Clear();

	delete pShaderRenderData;
}

void Direct3D11Render::DestroyTextureRenderData(void* pRenderData)
{
	TextureRenderDataD3D11* pTextureRenderData = static_cast<TextureRenderDataD3D11*>(pRenderData);

	DX_RELEASE(pTextureRenderData->m_pTexture);
	DX_RELEASE(pTextureRenderData->m_pSRV);

	delete pTextureRenderData;
}

void Direct3D11Render::DestroyRenderTargetRenderData(void* pRenderData)
{
	RenderTargetRenderDataD3D11* pRenderTargetRenderData = static_cast<RenderTargetRenderDataD3D11*>(pRenderData);

	for(uint32_t i = 0; i < pRenderTargetRenderData->m_numRTVs; ++i)
	{
		DX_RELEASE(pRenderTargetRenderData->m_RTVs[i]);
	}

	DX_RELEASE(pRenderTargetRenderData->m_pDSV);

	delete pRenderTargetRenderData;
}

void Direct3D11Render::SetMesh(Mesh& mesh)
{
	const MeshRenderDataD3D11* pMeshRenderData = static_cast<MeshRenderDataD3D11*>(mesh.m_pRenderData);
	m_pDeviceCtx->IASetIndexBuffer(pMeshRenderData->m_pIndexBuffer, (mesh.GetIndexSize() == sizeof(uint16_t)) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT, 0);
	const uint32_t vertexSize = mesh.GetVertexSize();
	const uint32_t vertexByteOffset = 0;
	m_pDeviceCtx->IASetVertexBuffers(0, 1, &pMeshRenderData->m_pVertexBuffer, &vertexSize, &vertexByteOffset);
}

void Direct3D11Render::UpdateDynamicMesh(DynamicMesh& dynamicMesh)
{
	MeshRenderDataD3D11* pMeshRenderData = static_cast<MeshRenderDataD3D11*>(dynamicMesh.m_pRenderData);

	if(dynamicMesh.m_isIndicesDirty)
	{
		UpdateD3D11Resource(pMeshRenderData->m_pIndexBuffer, dynamicMesh.m_indices.get(), dynamicMesh.GetSizeOfIndexData());
		dynamicMesh.m_isIndicesDirty = false;
	}

	if(dynamicMesh.m_isVerticesDirty)
	{
		UpdateD3D11Resource(pMeshRenderData->m_pVertexBuffer, dynamicMesh.m_vertices.get(), dynamicMesh.GetSizeOfVertexData());
		dynamicMesh.m_isVerticesDirty = false;
	}
}

void Direct3D11Render::SetTexture(uint8_t samplerIdx, const Texture& texture, EFilterMode filterMode, EAddressMode addressMode)
{
	ASSERT(samplerIdx < D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT);

	const TextureRenderDataD3D11* pTextureRenderData = static_cast<const TextureRenderDataD3D11*>(texture.m_pRenderData);
	m_pDeviceCtx->VSSetShaderResources(samplerIdx, 1, &pTextureRenderData->m_pSRV);
	m_pDeviceCtx->PSSetShaderResources(samplerIdx, 1, &pTextureRenderData->m_pSRV);

	static constexpr D3D11_TEXTURE_ADDRESS_MODE s_addressModeConversionTable[] =
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
	}

	samplerDesc.AddressU = s_addressModeConversionTable[(size_t)addressMode];
	samplerDesc.AddressV = s_addressModeConversionTable[(size_t)addressMode];
	samplerDesc.AddressW = s_addressModeConversionTable[(size_t)addressMode];
	samplerDesc.MaxAnisotropy = D3D11_MAX_MAXANISOTROPY;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	SetSamplerState(samplerIdx, samplerDesc);
}

void Direct3D11Render::SetRenderTarget(RenderTarget* pRenderTarget)
{
	D3D11_VIEWPORT viewport = { 0.0f, 0.0f, static_cast<FLOAT>(m_pActiveWindow->GetWidth()), static_cast<FLOAT>(m_pActiveWindow->GetHeight()), 0.0f, 1.0f };

	if(pRenderTarget != nullptr)
	{
		const RenderTargetRenderDataD3D11* pRenderTargetRenderData = static_cast<RenderTargetRenderDataD3D11*>(pRenderTarget->m_pRenderData);
		m_pDeviceCtx->OMSetRenderTargets(pRenderTargetRenderData->m_numRTVs, pRenderTargetRenderData->m_RTVs, pRenderTargetRenderData->m_pDSV);

		viewport.Width = static_cast<FLOAT>(pRenderTarget->GetWidth());
		viewport.Height = static_cast<FLOAT>(pRenderTarget->GetHeight());
	}
	else
	{
		m_pDeviceCtx->OMSetRenderTargets(1, &m_pFrameBufferRTV, m_pFrameBufferDSV);
	}

	m_pDeviceCtx->RSSetViewports(1, &viewport);
}

void Direct3D11Render::UpdateShaderParams(const Material& material)
{
	ShaderRenderDataD3D11* pVS = static_cast<ShaderRenderDataD3D11*>(material.m_vertexShader->m_pRenderData);
	UpdateShaderParams(material, pVS);

	ShaderRenderDataD3D11* pPS = static_cast<ShaderRenderDataD3D11*>(material.m_pixelShader->m_pRenderData);
	UpdateShaderParams(material, pPS);
}

void Direct3D11Render::UpdateShaderParams(const Material& material, ShaderRenderDataD3D11* pShaderRenderData)
{
	ShaderRenderDataD3D11::ConstantBufferData::Variable* pVariable = nullptr;

	for(size_t i = 0; i < pShaderRenderData->m_constantBuffers.Size(); ++i)
	{
		bool isChanged = false;
		ShaderRenderDataD3D11::ConstantBufferData& bufferData = pShaderRenderData->m_constantBuffersData[i];

		for(const Material::ShaderParam& shaderParam : material.GetShaderParams())
		{
			pVariable = bufferData.m_variables.Find(shaderParam.m_name);
			if(pVariable != nullptr)
			{
				isChanged |= pVariable->CheckAndUpdate(shaderParam.GetValue());
			}
		}

		pVariable = bufferData.m_variables.Find("matWorldView");
		if(pVariable != nullptr && pVariable->m_size == sizeof(DirectX::XMMATRIX))
		{
			const DirectX::XMMATRIX matWorldView = DirectX::XMMatrixMultiply(m_matWorld, m_matView);
			isChanged |= pVariable->CheckAndUpdate(&matWorldView.r[0]);
		}

		pVariable = bufferData.m_variables.Find("matWorldViewProj");
		if(pVariable != nullptr && pVariable->m_size == sizeof(DirectX::XMMATRIX))
		{
			const DirectX::XMMATRIX matWorldView = DirectX::XMMatrixMultiply(m_matWorld, m_matView);
			const DirectX::XMMATRIX matWorldViewProj = DirectX::XMMatrixMultiply(matWorldView, m_matProj);
			isChanged |= pVariable->CheckAndUpdate(&matWorldViewProj.r[0]);
		}

		pVariable = bufferData.m_variables.Find("matView");
		if(pVariable != nullptr)
		{
			isChanged |= pVariable->CheckAndUpdate(&m_matView.r[0]);
		}

		pVariable = bufferData.m_variables.Find("matViewInv");
		if(pVariable != nullptr)
		{
			const DirectX::XMMATRIX matViewInv = DirectX::XMMatrixInverse(nullptr, m_matView);
			isChanged |= pVariable->CheckAndUpdate(&matViewInv.r[0]);
		}

		pVariable = bufferData.m_variables.Find("camPos");
		if(pVariable != nullptr)
		{
			const DirectX::XMMATRIX matViewInv = DirectX::XMMatrixInverse(nullptr, m_matView);
			isChanged |= pVariable->CheckAndUpdate(&matViewInv.r[3]);
		}

		pVariable = bufferData.m_variables.Find("resolution");
		if(pVariable != nullptr && pVariable->m_size == 2 * sizeof(float))
		{
			float resolution[2] = {};
			if(m_pActiveRenderTarget != nullptr)
			{
				resolution[0] = m_pActiveRenderTarget->GetWidth();
				resolution[1] = m_pActiveRenderTarget->GetHeight();
			}
			else if(m_pActiveWindow != nullptr)
			{
				resolution[0] = m_pActiveWindow->GetWidth();
				resolution[1] = m_pActiveWindow->GetHeight();
			}

			isChanged |= pVariable->CheckAndUpdate(resolution);
		}

		if(isChanged)
		{
			m_pDeviceCtx->UpdateSubresource(pShaderRenderData->m_constantBuffers[i], 0, nullptr, bufferData.m_data.ptr.get(), 0, 0);
		}
	}
}

void Direct3D11Render::DoRender(Renderable& renderable, uint32_t indexOffset, uint32_t vertexOffset, uint32_t numIndices, uint32_t numVertices)
{
	RenderableRenderDataD3D11* pRenderableRenderData = static_cast<RenderableRenderDataD3D11*>(renderable.m_pRenderData);

	const ShaderRenderDataD3D11* pVS = static_cast<const ShaderRenderDataD3D11*>(renderable.m_material->m_vertexShader->m_pRenderData);
	if(m_pActiveVS != pVS)
	{
		m_pDeviceCtx->VSSetShader(pVS->m_pVertexShader, nullptr, 0);
		m_pDeviceCtx->VSSetConstantBuffers(0, static_cast<UINT>(pVS->m_constantBuffers.Size()), pVS->m_constantBuffers.Data());
		m_pActiveVS = pVS;
	}

	const ShaderRenderDataD3D11* pPS = static_cast<const ShaderRenderDataD3D11*>(renderable.m_material->m_pixelShader->m_pRenderData);
	if(m_pActivePS != pPS)
	{
		m_pDeviceCtx->PSSetShader(pPS->m_pPixelShader, nullptr, 0);
		m_pDeviceCtx->PSSetConstantBuffers(0, static_cast<UINT>(pPS->m_constantBuffers.Size()), pPS->m_constantBuffers.Data());
		m_pActivePS = pPS;
	}

	SetDepthStencilState();
	SetRasterizerState();
	SetBlendState();

	if(m_pActiveInputLayout != pRenderableRenderData->m_pInputLayout)
	{
		m_pDeviceCtx->IASetInputLayout(pRenderableRenderData->m_pInputLayout);
		m_pActiveInputLayout = pRenderableRenderData->m_pInputLayout;
	}

	if(numIndices != 0)
	{
		m_pDeviceCtx->DrawIndexed(numIndices, indexOffset, vertexOffset);
	}
	else
	{
		m_pDeviceCtx->Draw(numVertices, vertexOffset);
	}
}

bool Direct3D11Render::CreateFrameBuffers(uint32_t width, uint32_t height)
{
	// Create back buffer render target view
	ID3D11Texture2D* pFrameBufferTexture = nullptr;
	HRESULT hr = m_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pFrameBufferTexture));
	DX_CHECK_HRESULT(hr);
	hr = m_pDevice->CreateRenderTargetView(pFrameBufferTexture, nullptr, &m_pFrameBufferRTV);
	DX_CHECK_HRESULT(hr);
	DX_RELEASE(pFrameBufferTexture);

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

	ID3D11Texture2D* pDepthStencilTexture = nullptr;
	hr = m_pDevice->CreateTexture2D(&textureDesc, nullptr, &pDepthStencilTexture);
	DX_CHECK_HRESULT(hr);

	hr = m_pDevice->CreateDepthStencilView(pDepthStencilTexture, nullptr, &m_pFrameBufferDSV);
	if(FAILED(hr))
	{
		return false;
	}

	DX_RELEASE(pDepthStencilTexture);

	m_pDeviceCtx->OMSetRenderTargets(1, &m_pFrameBufferRTV, m_pFrameBufferDSV);

	return true;
}

void Direct3D11Render::UpdateD3D11Resource(ID3D11Resource* pResource, const void* pData, size_t size)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr = m_pDeviceCtx->Map(pResource, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(hr))
	{
		LOG_WARNING("Failed to map D3D11 resource");
		return;
	}

	memcpy(mappedResource.pData, pData, size);
	m_pDeviceCtx->Unmap(pResource, 0);
}

void Direct3D11Render::SetDepthStencilState()
{
	ID3D11DepthStencilState* pDepthStencilState = nullptr;

	uint64_t key = 0;
	key |= (uint64_t)m_depthStencilDesc.DepthEnable						<< 0;	// 1 bit
	key |= (uint64_t)m_depthStencilDesc.DepthWriteMask					<< 1;	// 1 bit
	key |= (uint64_t)m_depthStencilDesc.DepthFunc						<< 2;	// 4 bits
	key |= (uint64_t)m_depthStencilDesc.StencilEnable					<< 6;	// 1 bit
	key |= (uint64_t)m_depthStencilDesc.StencilReadMask					<< 7;	// 8 bits
	key |= (uint64_t)m_depthStencilDesc.StencilWriteMask				<< 15;	// 8 bits
	key |= (uint64_t)m_depthStencilDesc.FrontFace.StencilFailOp			<< 23;	// 4 bits
	key |= (uint64_t)m_depthStencilDesc.FrontFace.StencilDepthFailOp	<< 27;	// 4 bits
	key |= (uint64_t)m_depthStencilDesc.FrontFace.StencilPassOp			<< 31;	// 4 bits
	key |= (uint64_t)m_depthStencilDesc.FrontFace.StencilFunc			<< 35;	// 4 bits
	key |= (uint64_t)m_depthStencilDesc.BackFace.StencilFailOp			<< 39;	// 4 bits
	key |= (uint64_t)m_depthStencilDesc.BackFace.StencilDepthFailOp		<< 43;	// 4 bits
	key |= (uint64_t)m_depthStencilDesc.BackFace.StencilPassOp			<< 47;	// 4 bits
	key |= (uint64_t)m_depthStencilDesc.BackFace.StencilFunc			<< 51;	// 4 bits

	auto it = m_depthStencilStateCache.find(key);
	if(it != m_depthStencilStateCache.end())
	{
		pDepthStencilState = it->second;
	}
	else
	{
		HRESULT hr = m_pDevice->CreateDepthStencilState(&m_depthStencilDesc, &pDepthStencilState);
		DX_CHECK_HRESULT(hr);
		m_depthStencilStateCache.emplace(key, pDepthStencilState);
	}

	if(m_pActiveDepthStencilState != pDepthStencilState)
	{
		m_pDeviceCtx->OMSetDepthStencilState(pDepthStencilState, 0);
		m_pActiveDepthStencilState = pDepthStencilState;
	}
}

void Direct3D11Render::SetRasterizerState()
{
	ID3D11RasterizerState* pRasterizerState = nullptr;

	uint64_t key = 0;
	key |= (uint64_t)m_rasterizerDesc.FillMode				<< 0;	// 2 bits
	key |= (uint64_t)m_rasterizerDesc.CullMode				<< 2;	// 2 bits
	key |= (uint64_t)m_rasterizerDesc.FrontCounterClockwise	<< 4;	// 1 bit
	key |= (uint64_t)m_rasterizerDesc.DepthBias				<< 5;	// 32 bits
//	key |= (uint64_t)m_rasterizerDesc.DepthBiasClamp		<< // Constant
//	key |= (uint64_t)m_rasterizerDesc.SlopeScaledDepthBias	<< // Constant
	key |= (uint64_t)m_rasterizerDesc.DepthClipEnable 		<< 37;	// 1 bit
	key |= (uint64_t)m_rasterizerDesc.ScissorEnable			<< 38;	// 1 bit
	key |= (uint64_t)m_rasterizerDesc.MultisampleEnable		<< 39;	// 1 bit
	key |= (uint64_t)m_rasterizerDesc.AntialiasedLineEnable	<< 40;	// 1 bit

	auto it = m_rasterizerStateCache.find(key);
	if(it != m_rasterizerStateCache.end())
	{
		pRasterizerState = it->second;
	}
	else
	{
		HRESULT hr = m_pDevice->CreateRasterizerState(&m_rasterizerDesc, &pRasterizerState);
		DX_CHECK_HRESULT(hr);
		m_rasterizerStateCache.emplace(key, pRasterizerState);
	}

	if(m_pActiveRasterizerState != pRasterizerState)
	{
		m_pDeviceCtx->RSSetState(pRasterizerState);
		m_pActiveRasterizerState = pRasterizerState;
	}
}

void Direct3D11Render::SetBlendState()
{
	ID3D11BlendState* pBlendState = nullptr;

	uint64_t key = 0;
	key |= (uint64_t)m_blendDesc.AlphaToCoverageEnable					<< 0;	// 1 bit
	key |= (uint64_t)m_blendDesc.IndependentBlendEnable					<< 1;	// 1 bit
	key |= (uint64_t)m_blendDesc.RenderTarget[0].BlendEnable			<< 2;	// 1 bit
	key |= (uint64_t)m_blendDesc.RenderTarget[0].SrcBlend				<< 3;	// 5 bits
	key |= (uint64_t)m_blendDesc.RenderTarget[0].DestBlend				<< 8;	// 5 bits
	key |= (uint64_t)m_blendDesc.RenderTarget[0].BlendOp				<< 13;	// 3 bits
	key |= (uint64_t)m_blendDesc.RenderTarget[0].SrcBlendAlpha			<< 16;	// 5 bits
	key |= (uint64_t)m_blendDesc.RenderTarget[0].DestBlendAlpha			<< 21;	// 5 bits
	key |= (uint64_t)m_blendDesc.RenderTarget[0].BlendOpAlpha			<< 26;	// 3 bits
	key |= (uint64_t)m_blendDesc.RenderTarget[0].RenderTargetWriteMask	<< 29;	// 4 bits [0, 15], see D3D11_COLOR_WRITE_ENABLE

	auto it = m_blendStateCache.find(key);
	if(it != m_blendStateCache.end())
	{
		pBlendState = it->second;
	}
	else
	{
		HRESULT hr = m_pDevice->CreateBlendState(&m_blendDesc, &pBlendState);
		DX_CHECK_HRESULT(hr);
		m_blendStateCache.emplace(key, pBlendState);
	}

	if(m_pActiveBlendState != pBlendState)
	{
		m_pDeviceCtx->OMSetBlendState(pBlendState, m_blendFactor, 0xffffffff);
		m_pActiveBlendState = pBlendState;
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

	auto it = m_samplerStateCache.find(key);
	if(it != m_samplerStateCache.end())
	{
		samplerState = it->second;
	}
	else
	{
		HRESULT hr = m_pDevice->CreateSamplerState(&samplerDesc, &samplerState);
		DX_CHECK_HRESULT(hr);
		m_samplerStateCache.emplace(key, samplerState);
	}

	if(m_activeSamplerStates[samplerIdx] != samplerState)
	{
		m_pDeviceCtx->VSSetSamplers(samplerIdx, 1, &samplerState);
		m_pDeviceCtx->PSSetSamplers(samplerIdx, 1, &samplerState);
		m_activeSamplerStates[samplerIdx] = samplerState;
	}
}
