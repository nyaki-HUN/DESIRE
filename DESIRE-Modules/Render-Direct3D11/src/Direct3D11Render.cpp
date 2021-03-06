#include "stdafx_Direct3D11.h"
#include "Direct3D11Render.h"

#include "DirectXMathExt.h"
#include "RenderData/DeviceBufferRenderDataD3D11.h"
#include "RenderData/RenderableRenderDataD3D11.h"
#include "RenderData/RenderTargetRenderDataD3D11.h"
#include "RenderData/ShaderRenderDataD3D11.h"
#include "RenderData/TextureRenderDataD3D11.h"
#include "ToD3D11.h"

#include "Engine/Application/OSWindow.h"

#include "Engine/Core/FS/FileSystem.h"
#include "Engine/Core/Math/Matrix4.h"
#include "Engine/Core/String/StackString.h"

#include "Engine/Render/IndexBuffer.h"
#include "Engine/Render/Material.h"
#include "Engine/Render/Renderable.h"
#include "Engine/Render/RenderTarget.h"
#include "Engine/Render/Shader.h"
#include "Engine/Render/Texture.h"
#include "Engine/Render/VertexBuffer.h"

#include <d3dcompiler.h>

#if defined(_DEBUG)
	#define DX_CHECK_HRESULT(hr)		ASSERT(SUCCEEDED(hr))
#else
	#define DX_CHECK_HRESULT(hr)		DESIRE_UNUSED(hr)
#endif

static_assert(Render::kMaxTextureSamplers == D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT);

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
	m_spErrorVertexShader = std::make_unique<Shader>("vs_error");
	m_spErrorVertexShader->m_data = MemoryBuffer::CreateFromDataCopy(vs_error, sizeof(vs_error));
	m_spErrorPixelShader = std::make_unique<Shader>("ps_error");
	m_spErrorPixelShader->m_data = MemoryBuffer::CreateFromDataCopy(ps_error, sizeof(ps_error));

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
		DESIRE_ASIZEOF(featureLevels),
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

	m_spErrorVertexShader->m_pRenderData = CreateShaderRenderData(*m_spErrorVertexShader);
	m_spErrorPixelShader->m_pRenderData = CreateShaderRenderData(*m_spErrorPixelShader);

	return true;
}

void Direct3D11Render::UpdateRenderWindow(OSWindow& window)
{
	ID3D11RenderTargetView* nullViews[] = { nullptr };
	m_pDeviceCtx->OMSetRenderTargets(1, nullViews, nullptr);
	DX_SAFE_RELEASE(m_pFrameBufferDSV);
	DX_SAFE_RELEASE(m_pFrameBufferRTV);
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
	Unbind(*m_spErrorVertexShader);
	Unbind(*m_spErrorPixelShader);

	m_pActiveWindow = nullptr;
	m_pActiveIndexBuffer = nullptr;
	m_pActiveVertexBuffer = nullptr;
	m_pActiveRenderTarget = nullptr;

	m_pActiveVS = nullptr;
	m_pActivePS = nullptr;
	m_pActiveInputLayout = nullptr;
	m_pActiveDepthStencilState = nullptr;
	m_pActiveRasterizerState = nullptr;
	m_pActiveBlendState = nullptr;

	m_inputLayoutCache.clear();
	m_depthStencilStateCache.clear();

	for(auto& pair : m_rasterizerStateCache)
	{
		DX_SAFE_RELEASE(pair.second);
	}
	m_rasterizerStateCache.clear();

	for(auto& pair : m_blendStateCache)
	{
		DX_SAFE_RELEASE(pair.second);
	}
	m_blendStateCache.clear();

	for(auto& pair : m_samplerStateCache)
	{
		DX_SAFE_RELEASE(pair.second);
	}
	m_samplerStateCache.clear();
	memset(m_activeSamplerStates, 0, sizeof(m_activeSamplerStates));

	DX_SAFE_RELEASE(m_pFrameBufferDSV);
	DX_SAFE_RELEASE(m_pFrameBufferRTV);
	DX_SAFE_RELEASE(m_pSwapChain);
	DX_SAFE_RELEASE(m_pDeviceCtx);
	DX_SAFE_RELEASE(m_pDevice);
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

	if(m_pActiveRenderTarget)
	{
		RenderTargetRenderDataD3D11* pRenderTargetRenderData = static_cast<RenderTargetRenderDataD3D11*>(m_pActiveRenderTarget->m_pRenderData);
		if(pRenderTargetRenderData)
		{
			for(uint32_t i = 0; i < pRenderTargetRenderData->m_numRTVs; ++i)
			{
				m_pDeviceCtx->ClearRenderTargetView(pRenderTargetRenderData->m_RTVs[i], clearColor);
			}

			if(pRenderTargetRenderData->m_pDSV)
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

void Direct3D11Render::SetScissor(uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
	const D3D11_RECT rect = { x, y, x + width, y + height };
	m_pDeviceCtx->RSSetScissorRects(1, &rect);
}

void Direct3D11Render::SetWorldMatrix(const Matrix4& matrix)
{
	m_matWorld.r[0] = GetXMVECTOR(matrix.m_col0);
	m_matWorld.r[1] = GetXMVECTOR(matrix.m_col1);
	m_matWorld.r[2] = GetXMVECTOR(matrix.m_col2);
	m_matWorld.r[3] = GetXMVECTOR(matrix.m_col3);
}

void Direct3D11Render::SetViewProjectionMatrices(const Matrix4& viewMatrix, const Matrix4& projMatrix)
{
	m_matView.r[0] = GetXMVECTOR(viewMatrix.m_col0);
	m_matView.r[1] = GetXMVECTOR(viewMatrix.m_col1);
	m_matView.r[2] = GetXMVECTOR(viewMatrix.m_col2);
	m_matView.r[3] = GetXMVECTOR(viewMatrix.m_col3);

	m_matProj.r[0] = GetXMVECTOR(projMatrix.m_col0);
	m_matProj.r[1] = GetXMVECTOR(projMatrix.m_col1);
	m_matProj.r[2] = GetXMVECTOR(projMatrix.m_col2);
	m_matProj.r[3] = GetXMVECTOR(projMatrix.m_col3);
}

RenderData* Direct3D11Render::CreateRenderableRenderData(const Renderable& renderable)
{
	RenderableRenderDataD3D11* pRenderableRenderData = new RenderableRenderDataD3D11();

	// Input Layout
	{
		const ShaderRenderDataD3D11* pVS = static_cast<const ShaderRenderDataD3D11*>(renderable.m_spMaterial->m_spVertexShader->m_pRenderData);

		const Array<VertexBuffer::Layout>& vertexLayout = renderable.m_spVertexBuffer->GetVertexLayout();
		ASSERT(vertexLayout.Size() <= 9 && "It is possible to encode maximum of 9 vertex layouts into 64-bit");
		D3D11_INPUT_ELEMENT_DESC vertexElementDesc[static_cast<size_t>(VertexBuffer::EAttrib::Num)] = {};
		pRenderableRenderData->m_inputLayoutKey = 0;
		for(size_t i = 0; i < vertexLayout.Size(); ++i)
		{
			const VertexBuffer::Layout& layout = vertexLayout[i];
			vertexElementDesc[i] = ToD3D11(layout.attrib);
			vertexElementDesc[i].Format = ToD3D11(layout.type, layout.count);

			pRenderableRenderData->m_inputLayoutKey |= static_cast<uint64_t>(layout.attrib)		<< (i * 7 + 0);	// 4 bits
			pRenderableRenderData->m_inputLayoutKey |= static_cast<uint64_t>(layout.count - 1)	<< (i * 7 + 4);	// 2 bits [0, 3]
			pRenderableRenderData->m_inputLayoutKey |= static_cast<uint64_t>(layout.type)		<< (i * 7 + 6);	// 1 bit
		}

		auto iter = m_inputLayoutCache.find(pRenderableRenderData->m_inputLayoutKey);
		if(iter != m_inputLayoutCache.end())
		{
			pRenderableRenderData->m_pInputLayout = iter->second;
			pRenderableRenderData->m_pInputLayout->AddRef();
		}
		else
		{
			HRESULT hr = m_pDevice->CreateInputLayout(vertexElementDesc, static_cast<UINT>(vertexLayout.Size()), pVS->m_pShaderCode->GetBufferPointer(), pVS->m_pShaderCode->GetBufferSize(), &pRenderableRenderData->m_pInputLayout);
			DX_CHECK_HRESULT(hr);

			m_inputLayoutCache.emplace(pRenderableRenderData->m_inputLayoutKey, pRenderableRenderData->m_pInputLayout);
		}
	}

	// Rasterizer State
	{
		CD3D11_RASTERIZER_DESC rasterizerDesc(D3D11_DEFAULT);
		rasterizerDesc.DepthClipEnable = FALSE;
		rasterizerDesc.ScissorEnable = TRUE;
		rasterizerDesc.MultisampleEnable = TRUE;
		rasterizerDesc.AntialiasedLineEnable = TRUE;

		switch(renderable.m_spMaterial->m_cullMode)
		{
			case ECullMode::None:	rasterizerDesc.CullMode = D3D11_CULL_NONE; break;
			case ECullMode::CCW:	rasterizerDesc.CullMode = D3D11_CULL_BACK; break;
			case ECullMode::CW:		rasterizerDesc.CullMode = D3D11_CULL_FRONT; break;
		}

		pRenderableRenderData->m_rasterizerStateKey = 0
			| (uint64_t)rasterizerDesc.FillMode					<< 0	// 2 bits
			| (uint64_t)rasterizerDesc.CullMode					<< 2	// 2 bits
			| (uint64_t)rasterizerDesc.FrontCounterClockwise	<< 4	// 1 bit
			| (uint64_t)rasterizerDesc.DepthBias				<< 5	// 32 bits
//			| (uint64_t)rasterizerDesc.DepthBiasClamp			<< // Constant
//			| (uint64_t)rasterizerDesc.SlopeScaledDepthBias		<< // Constant
			| (uint64_t)rasterizerDesc.DepthClipEnable			<< 37	// 1 bit
			| (uint64_t)rasterizerDesc.ScissorEnable			<< 38	// 1 bit
			| (uint64_t)rasterizerDesc.MultisampleEnable		<< 39	// 1 bit
			| (uint64_t)rasterizerDesc.AntialiasedLineEnable	<< 40;	// 1 bit

		auto iter = m_rasterizerStateCache.find(pRenderableRenderData->m_rasterizerStateKey);
		if(iter != m_rasterizerStateCache.end())
		{
			pRenderableRenderData->m_pRasterizerState = iter->second;
			pRenderableRenderData->m_pRasterizerState->AddRef();
		}
		else
		{
			HRESULT hr = m_pDevice->CreateRasterizerState(&rasterizerDesc, &pRenderableRenderData->m_pRasterizerState);
			DX_CHECK_HRESULT(hr);
			m_rasterizerStateCache.emplace(pRenderableRenderData->m_rasterizerStateKey, pRenderableRenderData->m_pRasterizerState);
		}
	}

	// Blend State
	{
		CD3D11_BLEND_DESC blendDesc(D3D11_DEFAULT);
		if(renderable.m_spMaterial->m_isBlendEnabled)
		{
			blendDesc.RenderTarget[0].BlendEnable = TRUE;
			blendDesc.RenderTarget[0].SrcBlend = ToD3D11(renderable.m_spMaterial->m_srcBlendRGB);
			blendDesc.RenderTarget[0].DestBlend = ToD3D11(renderable.m_spMaterial->m_destBlendRGB);
			blendDesc.RenderTarget[0].BlendOp = ToD3D11(renderable.m_spMaterial->m_blendOpRGB);
			blendDesc.RenderTarget[0].SrcBlendAlpha = ToD3D11(renderable.m_spMaterial->m_srcBlendAlpha);
			blendDesc.RenderTarget[0].DestBlendAlpha = ToD3D11(renderable.m_spMaterial->m_destBlendAlpha);
			blendDesc.RenderTarget[0].BlendOpAlpha = ToD3D11(renderable.m_spMaterial->m_blendOpAlpha);
		}

		static_assert(static_cast<uint8_t>(EColorWrite::Red) == D3D11_COLOR_WRITE_ENABLE_RED);
		static_assert(static_cast<uint8_t>(EColorWrite::Green) == D3D11_COLOR_WRITE_ENABLE_GREEN);
		static_assert(static_cast<uint8_t>(EColorWrite::Blue) == D3D11_COLOR_WRITE_ENABLE_BLUE);
		static_assert(static_cast<uint8_t>(EColorWrite::Alpha) == D3D11_COLOR_WRITE_ENABLE_ALPHA);

		blendDesc.RenderTarget[0].RenderTargetWriteMask = static_cast<uint8_t>(renderable.m_spMaterial->m_colorWriteMask);

		pRenderableRenderData->m_blendStateKey = 0
			| (uint64_t)blendDesc.AlphaToCoverageEnable					<< 0	// 1 bit
			| (uint64_t)blendDesc.IndependentBlendEnable				<< 1	// 1 bit
			| (uint64_t)blendDesc.RenderTarget[0].BlendEnable			<< 2	// 1 bit
			| (uint64_t)blendDesc.RenderTarget[0].SrcBlend				<< 3	// 5 bits
			| (uint64_t)blendDesc.RenderTarget[0].DestBlend				<< 8	// 5 bits
			| (uint64_t)blendDesc.RenderTarget[0].BlendOp				<< 13	// 3 bits
			| (uint64_t)blendDesc.RenderTarget[0].SrcBlendAlpha			<< 16	// 5 bits
			| (uint64_t)blendDesc.RenderTarget[0].DestBlendAlpha		<< 21	// 5 bits
			| (uint64_t)blendDesc.RenderTarget[0].BlendOpAlpha			<< 26	// 3 bits
			| (uint64_t)blendDesc.RenderTarget[0].RenderTargetWriteMask	<< 29;	// 4 bits

		auto iter = m_blendStateCache.find(pRenderableRenderData->m_blendStateKey);
		if(iter != m_blendStateCache.end())
		{
			pRenderableRenderData->m_pBlendState = iter->second;
			pRenderableRenderData->m_pBlendState->AddRef();
		}
		else
		{
			HRESULT hr = m_pDevice->CreateBlendState(&blendDesc, &pRenderableRenderData->m_pBlendState);
			DX_CHECK_HRESULT(hr);
			m_blendStateCache.emplace(pRenderableRenderData->m_blendStateKey, pRenderableRenderData->m_pBlendState);
		}
	}

	// Depth Stencil State
	{
		CD3D11_DEPTH_STENCIL_DESC depthStencilDesc(D3D11_DEFAULT);

		switch(renderable.m_spMaterial->m_depthTest)
		{
			case EDepthTest::Disabled:		depthStencilDesc.DepthEnable = FALSE; break;
			case EDepthTest::Less:			depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS; break;
			case EDepthTest::LessEqual:		depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL; break;
			case EDepthTest::Greater:		depthStencilDesc.DepthFunc = D3D11_COMPARISON_GREATER; break;
			case EDepthTest::GreaterEqual:	depthStencilDesc.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL; break;
			case EDepthTest::Equal:			depthStencilDesc.DepthFunc = D3D11_COMPARISON_EQUAL; break;
			case EDepthTest::NotEqual:		depthStencilDesc.DepthFunc = D3D11_COMPARISON_NOT_EQUAL; break;
		}

		depthStencilDesc.DepthWriteMask = renderable.m_spMaterial->m_isDepthWriteEnabled ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;

		pRenderableRenderData->m_depthStencilStateKey = 0
			| (uint64_t)depthStencilDesc.DepthEnable					<< 0	// 1 bit
			| (uint64_t)depthStencilDesc.DepthWriteMask					<< 1	// 1 bit
			| (uint64_t)depthStencilDesc.DepthFunc						<< 2	// 4 bits
			| (uint64_t)depthStencilDesc.StencilEnable					<< 6	// 1 bit
			| (uint64_t)depthStencilDesc.StencilReadMask				<< 7	// 8 bits
			| (uint64_t)depthStencilDesc.StencilWriteMask				<< 15	// 8 bits
			| (uint64_t)depthStencilDesc.FrontFace.StencilFailOp		<< 23	// 4 bits
			| (uint64_t)depthStencilDesc.FrontFace.StencilDepthFailOp	<< 27	// 4 bits
			| (uint64_t)depthStencilDesc.FrontFace.StencilPassOp		<< 31	// 4 bits
			| (uint64_t)depthStencilDesc.FrontFace.StencilFunc			<< 35	// 4 bits
			| (uint64_t)depthStencilDesc.BackFace.StencilFailOp			<< 39	// 4 bits
			| (uint64_t)depthStencilDesc.BackFace.StencilDepthFailOp	<< 43	// 4 bits
			| (uint64_t)depthStencilDesc.BackFace.StencilPassOp			<< 47	// 4 bits
			| (uint64_t)depthStencilDesc.BackFace.StencilFunc			<< 51;	// 4 bits

		auto iter = m_depthStencilStateCache.find(pRenderableRenderData->m_depthStencilStateKey);
		if(iter != m_depthStencilStateCache.end())
		{
			pRenderableRenderData->m_pDepthStencilState = iter->second;
			pRenderableRenderData->m_pDepthStencilState->AddRef();
		}
		else
		{
			HRESULT hr = m_pDevice->CreateDepthStencilState(&depthStencilDesc, &pRenderableRenderData->m_pDepthStencilState);
			DX_CHECK_HRESULT(hr);

			m_depthStencilStateCache.emplace(pRenderableRenderData->m_depthStencilStateKey, pRenderableRenderData->m_pDepthStencilState);
		}
	}

	return pRenderableRenderData;
}

RenderData* Direct3D11Render::CreateIndexBufferRenderData(const IndexBuffer& indexBuffer)
{
	return CreateDeviceBufferRenderData(indexBuffer, D3D11_BIND_INDEX_BUFFER);
}

RenderData* Direct3D11Render::CreateVertexBufferRenderData(const VertexBuffer& vertexBuffer)
{
	return CreateDeviceBufferRenderData(vertexBuffer, D3D11_BIND_VERTEX_BUFFER);
}

RenderData* Direct3D11Render::CreateShaderRenderData(const Shader& shader)
{
	ShaderRenderDataD3D11* pShaderRenderData = new ShaderRenderDataD3D11();

	D3D_SHADER_MACRO shaderDefines[32] = {};
	ASSERT(shader.m_defines.size() < DESIRE_ASIZEOF(shaderDefines));
	D3D_SHADER_MACRO* pShaderDefine = &shaderDefines[0];
	for(const String& define : shader.m_defines)
	{
		pShaderDefine->Name = define.Str();
		pShaderDefine->Definition = "1";
		pShaderDefine++;
	}

	StackString<DESIRE_MAX_PATH_LEN> filenameWithPath = FileSystem::Get().GetAppDirectory();
	AppendShaderFilenameWithPath(filenameWithPath, shader.m_name);

	const bool isVertexShader = shader.m_name.StartsWith("vs_");
	UINT compileFlags = 0;
/**/compileFlags = D3DCOMPILE_SKIP_OPTIMIZATION;

	ComPtr<ID3DBlob> spErrorBlob = nullptr;
	HRESULT hr = D3DCompile(shader.m_data.GetData(),	// pSrcData
		shader.m_data.GetSize(),						// SrcDataSize
		filenameWithPath.Str(),							// pSourceName
		shaderDefines,									// pDefines
		D3D_COMPILE_STANDARD_FILE_INCLUDE,				// pInclude
		"main",											// pEntrypoint
		isVertexShader ? "vs_5_0" : "ps_5_0",			// pTarget
		compileFlags,									// D3DCOMPILE flags
		0,												// D3DCOMPILE_EFFECT flags
		&pShaderRenderData->m_pShaderCode,				// ppCode
		spErrorBlob.GetAddressOf());					// ppErrorMsgs
	if(FAILED(hr))
	{
		if(spErrorBlob)
		{
			LOG_ERROR("Shader compile error:\n%s", static_cast<const char*>(spErrorBlob->GetBufferPointer()));
		}

		const ShaderRenderDataD3D11* pErrorShaderRenderData = static_cast<const ShaderRenderDataD3D11*>(isVertexShader ? m_spErrorVertexShader->m_pRenderData : m_spErrorPixelShader->m_pRenderData);
		pShaderRenderData->m_pShaderCode = pErrorShaderRenderData->m_pShaderCode;
		pShaderRenderData->m_pShaderCode->AddRef();
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

	ComPtr<ID3D11ShaderReflection> spReflection = nullptr;
	hr = D3DReflect(pShaderRenderData->m_pShaderCode->GetBufferPointer(), pShaderRenderData->m_pShaderCode->GetBufferSize(), IID_PPV_ARGS(spReflection.GetAddressOf()));
	if(FAILED(hr))
	{
		LOG_ERROR("D3DReflect failed 0x%08x\n", static_cast<uint32_t>(hr));
	}

	D3D11_SHADER_DESC shaderDesc;
	hr = spReflection->GetDesc(&shaderDesc);
	if(FAILED(hr))
	{
		LOG_ERROR("ID3D11ShaderReflection::GetDesc failed 0x%08x\n", static_cast<uint32_t>(hr));
	}

	pShaderRenderData->m_constantBuffers.SetSize(shaderDesc.ConstantBuffers);
	pShaderRenderData->m_constantBuffersData.SetSize(shaderDesc.ConstantBuffers);
	for(uint32_t i = 0; i < shaderDesc.ConstantBuffers; ++i)
	{
		ID3D11ShaderReflectionConstantBuffer* pConstantBuffer = spReflection->GetConstantBufferByIndex(i);
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
			bufferData.data = MemoryBuffer(shaderBufferDesc.Size);

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
					bufferData.variables.Insert(String(varDesc.Name, strlen(varDesc.Name)), { bufferData.data.GetData() + varDesc.StartOffset, varDesc.Size });
				}
			}
		}
	}

	return pShaderRenderData;
}

RenderData* Direct3D11Render::CreateTextureRenderData(const Texture& texture)
{
	TextureRenderDataD3D11* pTextureRenderData = new TextureRenderDataD3D11();

	const bool isRenderTarget = (texture.GetData() == nullptr);

	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = texture.GetWidth();
	textureDesc.Height = texture.GetHeight();
	textureDesc.MipLevels = texture.GetNumMipLevels();
	textureDesc.ArraySize = 1;
	textureDesc.Format = ToD3D11(texture.GetFormat());
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

RenderData* Direct3D11Render::CreateRenderTargetRenderData(const RenderTarget& renderTarget)
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

void Direct3D11Render::OnDestroyRenderableRenderData(RenderData* pRenderData)
{
	RenderableRenderDataD3D11* pRenderableRenderData = static_cast<RenderableRenderDataD3D11*>(pRenderData);

	UINT refCount = pRenderableRenderData->m_pInputLayout->Release();
	if(refCount == 0)
	{
		if(m_pActiveInputLayout == pRenderableRenderData->m_pInputLayout)
		{
			m_pActiveInputLayout = nullptr;
		}

		m_inputLayoutCache.erase(pRenderableRenderData->m_inputLayoutKey);
	}

	refCount = pRenderableRenderData->m_pRasterizerState->Release();
	if(refCount == 0)
	{
		if(m_pActiveRasterizerState != pRenderableRenderData->m_pRasterizerState)
		{
			m_pActiveRasterizerState = nullptr;
		}

		m_rasterizerStateCache.erase(pRenderableRenderData->m_rasterizerStateKey);
	}

	refCount = pRenderableRenderData->m_pBlendState->Release();
	if(refCount == 0)
	{
		if(m_pActiveBlendState == pRenderableRenderData->m_pBlendState)
		{
			m_pActiveBlendState = nullptr;
		}

		m_blendStateCache.erase(pRenderableRenderData->m_blendStateKey);
	}

	refCount = pRenderableRenderData->m_pDepthStencilState->Release();
	if(refCount == 0)
	{
		if(m_pActiveDepthStencilState == pRenderableRenderData->m_pDepthStencilState)
		{
			m_pActiveDepthStencilState = nullptr;
		}

		m_depthStencilStateCache.erase(pRenderableRenderData->m_depthStencilStateKey);
	}
}

void Direct3D11Render::OnDestroyShaderRenderData(RenderData* pRenderData)
{
	if(m_pActiveVS == pRenderData)
	{
		m_pActiveVS = nullptr;
	}

	if(m_pActivePS == pRenderData)
	{
		m_pActivePS = nullptr;
	}
}

void Direct3D11Render::SetIndexBuffer(IndexBuffer& indexBuffer)
{
	const DeviceBufferRenderDataD3D11* pDeviceBufferRenderData = static_cast<DeviceBufferRenderDataD3D11*>(indexBuffer.m_pRenderData);

	const DXGI_FORMAT format = (indexBuffer.GetIndexSize() == sizeof(uint16_t)) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
	m_pDeviceCtx->IASetIndexBuffer(pDeviceBufferRenderData->m_pBuffer, format, 0);
}

void Direct3D11Render::SetVertexBuffer(VertexBuffer& vertexBuffer)
{
	const DeviceBufferRenderDataD3D11* pDeviceBufferRenderData = static_cast<DeviceBufferRenderDataD3D11*>(vertexBuffer.m_pRenderData);

	const uint32_t vertexSize = vertexBuffer.GetVertexSize();
	const uint32_t vertexByteOffset = 0;
	m_pDeviceCtx->IASetVertexBuffers(0, 1, &pDeviceBufferRenderData->m_pBuffer, &vertexSize, &vertexByteOffset);
}

void Direct3D11Render::SetRenderTarget(RenderTarget* pRenderTarget)
{
	D3D11_VIEWPORT viewport = { 0.0f, 0.0f, static_cast<FLOAT>(m_pActiveWindow->GetWidth()), static_cast<FLOAT>(m_pActiveWindow->GetHeight()), 0.0f, 1.0f };

	if(pRenderTarget)
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

void Direct3D11Render::UpdateDeviceBuffer(DeviceBuffer& deviceBuffer)
{
	DeviceBufferRenderDataD3D11* pDeviceBufferRenderData = static_cast<DeviceBufferRenderDataD3D11*>(deviceBuffer.m_pRenderData);
	UpdateD3D11Resource(pDeviceBufferRenderData->m_pBuffer, deviceBuffer.GetData(), deviceBuffer.GetDataSize());
	deviceBuffer.SetDirty(false);
}

void Direct3D11Render::UpdateShaderParams(const Material& material)
{
	ShaderRenderDataD3D11* pVS = static_cast<ShaderRenderDataD3D11*>(material.m_spVertexShader->m_pRenderData);
	UpdateShaderParams(material, pVS);

	ShaderRenderDataD3D11* pPS = static_cast<ShaderRenderDataD3D11*>(material.m_spPixelShader->m_pRenderData);
	UpdateShaderParams(material, pPS);

	ASSERT(material.GetTextures().Size() <= Render::kMaxTextureSamplers);
	uint8_t samplerIdx = 0;
	for(const Material::TextureInfo& textureInfo : material.GetTextures())
	{
		const TextureRenderDataD3D11* pTextureRenderData = static_cast<const TextureRenderDataD3D11*>(textureInfo.spTexture->m_pRenderData);

		m_pDeviceCtx->VSSetShaderResources(samplerIdx, 1, &pTextureRenderData->m_pSRV);
		m_pDeviceCtx->PSSetShaderResources(samplerIdx, 1, &pTextureRenderData->m_pSRV);

		D3D11_TEXTURE_ADDRESS_MODE addressMode = D3D11_TEXTURE_ADDRESS_WRAP;
		switch(textureInfo.addressMode)
		{
			case EAddressMode::Repeat:			addressMode = D3D11_TEXTURE_ADDRESS_WRAP; break;
			case EAddressMode::Clamp:			addressMode = D3D11_TEXTURE_ADDRESS_CLAMP; break;
			case EAddressMode::MirroredRepeat:	addressMode = D3D11_TEXTURE_ADDRESS_MIRROR; break;
			case EAddressMode::MirrorOnce:		addressMode = D3D11_TEXTURE_ADDRESS_MIRROR_ONCE; break;
			case EAddressMode::Border:			addressMode = D3D11_TEXTURE_ADDRESS_BORDER; break;
		}

		D3D11_SAMPLER_DESC samplerDesc = {};
		switch(textureInfo.filterMode)
		{
			case EFilterMode::Point:		samplerDesc.Filter = D3D11_ENCODE_BASIC_FILTER(D3D11_FILTER_TYPE_POINT, D3D11_FILTER_TYPE_POINT, D3D11_FILTER_TYPE_POINT, D3D11_FILTER_REDUCTION_TYPE_STANDARD); break;
			case EFilterMode::Bilinear:		samplerDesc.Filter = D3D11_ENCODE_BASIC_FILTER(D3D11_FILTER_TYPE_LINEAR, D3D11_FILTER_TYPE_LINEAR, D3D11_FILTER_TYPE_POINT, D3D11_FILTER_REDUCTION_TYPE_STANDARD); break;
			case EFilterMode::Trilinear:	samplerDesc.Filter = D3D11_ENCODE_BASIC_FILTER(D3D11_FILTER_TYPE_LINEAR, D3D11_FILTER_TYPE_LINEAR, D3D11_FILTER_TYPE_LINEAR, D3D11_FILTER_REDUCTION_TYPE_STANDARD); break;
			case EFilterMode::Anisotropic:	samplerDesc.Filter = D3D11_ENCODE_ANISOTROPIC_FILTER(D3D11_FILTER_REDUCTION_TYPE_STANDARD); break;
		}

		samplerDesc.AddressU = addressMode;
		samplerDesc.AddressV = addressMode;
		samplerDesc.AddressW = addressMode;
		samplerDesc.MaxAnisotropy = D3D11_MAX_MAXANISOTROPY;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

		SetSamplerState(samplerIdx, samplerDesc);
		samplerIdx++;
	}
}

void Direct3D11Render::UpdateShaderParams(const Material& material, ShaderRenderDataD3D11* pShaderRenderData)
{
	ShaderRenderDataD3D11::ConstantBufferVariable* pVariable = nullptr;

	for(size_t i = 0; i < pShaderRenderData->m_constantBuffersData.Size(); ++i)
	{
		bool isChanged = false;
		ShaderRenderDataD3D11::ConstantBufferData& bufferData = pShaderRenderData->m_constantBuffersData[i];

		for(const Material::ShaderParam& shaderParam : material.GetShaderParams())
		{
			pVariable = bufferData.variables.Find(shaderParam.name);
			if(pVariable)
			{
				isChanged |= pVariable->CheckAndUpdate(shaderParam.GetValue());
			}
		}

		pVariable = bufferData.variables.Find("matWorldView");
		if(pVariable && pVariable->size == sizeof(DirectX::XMMATRIX))
		{
			const DirectX::XMMATRIX matWorldView = DirectX::XMMatrixMultiply(m_matWorld, m_matView);
			isChanged |= pVariable->CheckAndUpdate(&matWorldView.r[0]);
		}

		pVariable = bufferData.variables.Find("matWorldViewProj");
		if(pVariable && pVariable->size == sizeof(DirectX::XMMATRIX))
		{
			const DirectX::XMMATRIX matWorldView = DirectX::XMMatrixMultiply(m_matWorld, m_matView);
			const DirectX::XMMATRIX matWorldViewProj = DirectX::XMMatrixMultiply(matWorldView, m_matProj);
			isChanged |= pVariable->CheckAndUpdate(&matWorldViewProj.r[0]);
		}

		pVariable = bufferData.variables.Find("matView");
		if(pVariable)
		{
			isChanged |= pVariable->CheckAndUpdate(&m_matView.r[0]);
		}

		pVariable = bufferData.variables.Find("matViewInv");
		if(pVariable)
		{
			const DirectX::XMMATRIX matViewInv = DirectX::XMMatrixInverse(nullptr, m_matView);
			isChanged |= pVariable->CheckAndUpdate(&matViewInv.r[0]);
		}

		pVariable = bufferData.variables.Find("camPos");
		if(pVariable)
		{
			const DirectX::XMMATRIX matViewInv = DirectX::XMMatrixInverse(nullptr, m_matView);
			isChanged |= pVariable->CheckAndUpdate(&matViewInv.r[3]);
		}

		pVariable = bufferData.variables.Find("resolution");
		if(pVariable && pVariable->size == 2 * sizeof(float))
		{
			float resolution[2] = {};
			if(m_pActiveRenderTarget)
			{
				resolution[0] = m_pActiveRenderTarget->GetWidth();
				resolution[1] = m_pActiveRenderTarget->GetHeight();
			}
			else if(m_pActiveWindow)
			{
				resolution[0] = m_pActiveWindow->GetWidth();
				resolution[1] = m_pActiveWindow->GetHeight();
			}

			isChanged |= pVariable->CheckAndUpdate(resolution);
		}

		if(isChanged)
		{
			m_pDeviceCtx->UpdateSubresource(pShaderRenderData->m_constantBuffers[i], 0, nullptr, bufferData.data.GetData(), 0, 0);
		}
	}
}

void Direct3D11Render::DoRender(Renderable& renderable, uint32_t indexOffset, uint32_t vertexOffset, uint32_t numIndices, uint32_t numVertices)
{
	RenderableRenderDataD3D11* pRenderableRenderData = static_cast<RenderableRenderDataD3D11*>(renderable.m_pRenderData);

	const ShaderRenderDataD3D11* pVS = static_cast<const ShaderRenderDataD3D11*>(renderable.m_spMaterial->m_spVertexShader->m_pRenderData);
	if(m_pActiveVS != pVS)
	{
		m_pDeviceCtx->VSSetShader(pVS->m_pVertexShader, nullptr, 0);
		m_pDeviceCtx->VSSetConstantBuffers(0, static_cast<UINT>(pVS->m_constantBuffers.Size()), pVS->m_constantBuffers.Data());
		m_pActiveVS = pVS;
	}

	const ShaderRenderDataD3D11* pPS = static_cast<const ShaderRenderDataD3D11*>(renderable.m_spMaterial->m_spPixelShader->m_pRenderData);
	if(m_pActivePS != pPS)
	{
		m_pDeviceCtx->PSSetShader(pPS->m_pPixelShader, nullptr, 0);
		m_pDeviceCtx->PSSetConstantBuffers(0, static_cast<UINT>(pPS->m_constantBuffers.Size()), pPS->m_constantBuffers.Data());
		m_pActivePS = pPS;
	}

	if(m_pActiveInputLayout != pRenderableRenderData->m_pInputLayout)
	{
		m_pDeviceCtx->IASetInputLayout(pRenderableRenderData->m_pInputLayout);
		m_pActiveInputLayout = pRenderableRenderData->m_pInputLayout;
	}

	if(m_pActiveRasterizerState != pRenderableRenderData->m_pRasterizerState)
	{
		m_pDeviceCtx->RSSetState(pRenderableRenderData->m_pRasterizerState);
		m_pActiveRasterizerState = pRenderableRenderData->m_pRasterizerState;
	}

	if(m_pActiveBlendState != pRenderableRenderData->m_pBlendState)
	{
		m_pDeviceCtx->OMSetBlendState(pRenderableRenderData->m_pBlendState, m_blendFactor, UINT32_MAX);
		m_pActiveBlendState = pRenderableRenderData->m_pBlendState;
	}

	if(m_pActiveDepthStencilState != pRenderableRenderData->m_pDepthStencilState)
	{
		m_pDeviceCtx->OMSetDepthStencilState(pRenderableRenderData->m_pDepthStencilState, 0);
		m_pActiveDepthStencilState = pRenderableRenderData->m_pDepthStencilState;
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
	ComPtr<ID3D11Texture2D> spFrameBufferTexture = nullptr;
	HRESULT hr = m_pSwapChain->GetBuffer(0, IID_PPV_ARGS(spFrameBufferTexture.GetAddressOf()));
	if(FAILED(hr))
	{
		return false;
	}

	hr = m_pDevice->CreateRenderTargetView(spFrameBufferTexture.Get(), nullptr, &m_pFrameBufferRTV);
	if(FAILED(hr))
	{
		return false;
	}

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

	ComPtr<ID3D11Texture2D> spDepthStencilTexture = nullptr;
	hr = m_pDevice->CreateTexture2D(&textureDesc, nullptr, spDepthStencilTexture.GetAddressOf());
	if(FAILED(hr))
	{
		return false;
	}

	hr = m_pDevice->CreateDepthStencilView(spDepthStencilTexture.Get(), nullptr, &m_pFrameBufferDSV);
	if(FAILED(hr))
	{
		return false;
	}

	m_pDeviceCtx->OMSetRenderTargets(1, &m_pFrameBufferRTV, m_pFrameBufferDSV);

	return true;
}

RenderData* Direct3D11Render::CreateDeviceBufferRenderData(const DeviceBuffer& deviceBuffer, uint32_t bindFlags)
{
	DeviceBufferRenderDataD3D11* pDeviceBufferRenderData = new DeviceBufferRenderDataD3D11();

	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.ByteWidth = deviceBuffer.GetDataSize();
	bufferDesc.BindFlags = bindFlags;

	if(deviceBuffer.GetFlags() & DeviceBuffer::STATIC)
	{
		bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	}
	else
	{
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}

	D3D11_SUBRESOURCE_DATA subResourceData = {};
	subResourceData.pSysMem = deviceBuffer.GetData();

	HRESULT hr = m_pDevice->CreateBuffer(&bufferDesc, &subResourceData, &pDeviceBufferRenderData->m_pBuffer);
	DX_CHECK_HRESULT(hr);

	return pDeviceBufferRenderData;
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
