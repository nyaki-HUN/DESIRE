#include "stdafx_Direct3D12.h"
#include "Direct3D12Render.h"

#include "DirectXMathExt.h"
#include "MeshRenderDataD3D12.h"
#include "RenderTargetRenderDataD3D12.h"
#include "ShaderRenderDataD3D12.h"
#include "TextureRenderDataD3D12.h"

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
#include <dxgi1_4.h>

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

Direct3D12Render::Direct3D12Render()
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

	// Stencil test parameters
	m_depthStencilDesc.StencilEnable = FALSE;
	m_depthStencilDesc.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
	m_depthStencilDesc.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;
	// Stencil operations if pixel is front-facing
	m_depthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	m_depthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	m_depthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	m_depthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	// Stencil operations if pixel is back-facing
	m_depthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	m_depthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	m_depthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	m_depthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;

	m_rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
	m_rasterizerDesc.DepthClipEnable = TRUE;
	m_rasterizerDesc.MultisampleEnable = TRUE;
	m_rasterizerDesc.AntialiasedLineEnable = TRUE;

	m_blendDesc.RenderTarget[0].BlendEnable = TRUE;
	m_blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	m_blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	m_blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	m_blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_SRC_ALPHA;
	m_blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;
	m_blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	m_blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	m_matWorld = DirectX::XMMatrixIdentity();
	m_matView = DirectX::XMMatrixIdentity();
	m_matProj = DirectX::XMMatrixIdentity();
}

Direct3D12Render::~Direct3D12Render()
{
}

bool Direct3D12Render::Init(OSWindow& mainWindow)
{
	HRESULT hr;

#if defined(_DEBUG)
	// Enable the D3D12 debug layer.
	ID3D12Debug* pDebugController = nullptr;
	hr = D3D12GetDebugInterface(IID_PPV_ARGS(&pDebugController));
	if(SUCCEEDED(hr))
	{
		pDebugController->EnableDebugLayer();
		DX_RELEASE(pDebugController);
	}
#endif

	IDXGIFactory4* pFactory = nullptr;
	hr = CreateDXGIFactory1(IID_PPV_ARGS(&pFactory));
	if(FAILED(hr))
	{
		return false;
	}

	// Create device
	UINT adapterIndex = 0;
	IDXGIAdapter1* pAdapter = nullptr;
	while(pFactory->EnumAdapters1(adapterIndex, &pAdapter) != DXGI_ERROR_NOT_FOUND)
	{
		DXGI_ADAPTER_DESC1 desc;
		pAdapter->GetDesc1(&desc);

		if(desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
		{
			// Skip SW device
			continue;
		}

		hr = D3D12CreateDevice(
			pAdapter,
			D3D_FEATURE_LEVEL_12_1,
			IID_PPV_ARGS(&m_pDevice)
		);

		if(SUCCEEDED(hr))
		{
			break;
		}

		adapterIndex++;
	}

	if(m_pDevice == nullptr)
	{
		return false;
	}

	// Create command queue
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc = {};
	commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	commandQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

	hr = m_pDevice->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&m_pCommandQueue));
	if(FAILED(hr))
	{
		return false;
	}

	// Create swap chain
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.Width = mainWindow.GetWidth();
	swapChainDesc.Height = mainWindow.GetHeight();
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.Stereo = FALSE;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = kFrameBufferCount;
	swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;

	IDXGISwapChain1* pTmpSwapChain = nullptr;
	hr = pFactory->CreateSwapChainForHwnd(m_pCommandQueue, static_cast<HWND>(mainWindow.GetHandle()), &swapChainDesc, nullptr, nullptr, &pTmpSwapChain);
	if(FAILED(hr))
	{
		return false;
	}

	hr = pTmpSwapChain->QueryInterface(IID_PPV_ARGS(&m_pSwapChain));
	DX_RELEASE(pTmpSwapChain);
	if(FAILED(hr))
	{
		return false;
	}

	hr = pFactory->MakeWindowAssociation(static_cast<HWND>(mainWindow.GetHandle()), DXGI_MWA_NO_ALT_ENTER);
	DX_CHECK_HRESULT(hr);

	bool succeeded = CreateFrameBuffers(mainWindow.GetWidth(), mainWindow.GetHeight());
	if(!succeeded)
	{
		return false;
	}

	// Create command list
	hr = m_pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_frameBuffers[0].m_pCommandAllocator, nullptr, IID_PPV_ARGS(&m_pCmdList));
	if(FAILED(hr))
	{
		return false;
	}

	// Command lists are created in the recording state, but we will set it up for recording again so close it now
	hr = m_pCmdList->Close();
	if(FAILED(hr))
	{
		return false;
	}

	m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if(m_fenceEvent == nullptr)
	{
		return false;
	}

	m_activeFrameBufferIdx = m_pSwapChain->GetCurrentBackBufferIndex();

	Bind(*m_errorVertexShader);
	Bind(*m_errorPixelShader);

	return true;
}

void Direct3D12Render::UpdateRenderWindow(OSWindow& window)
{
	HRESULT hr = m_pSwapChain->ResizeBuffers(0, window.GetWidth(), window.GetHeight(), DXGI_FORMAT_UNKNOWN, 0);
	DX_CHECK_HRESULT(hr);

	WaitForPreviousFrame();

	for(uint32_t i = 0; i < kFrameBufferCount; ++i)
	{
		DX_RELEASE(m_frameBuffers[i].m_pRenderTarget);
	}

	bool succeeded = CreateFrameBuffers(window.GetWidth(), window.GetHeight());
	ASSERT(succeeded);
}

void Direct3D12Render::Kill()
{
	// Wait for the GPU to finish all frames
	for(uint32_t i = 0; i < kFrameBufferCount; ++i)
	{
		m_activeFrameBufferIdx = i;
		WaitForPreviousFrame();
	}

	Unbind(*m_errorVertexShader);
	Unbind(*m_errorPixelShader);

	m_pActiveWindow = nullptr;
	m_pActiveMesh = nullptr;
	m_pActiveVertexShader = nullptr;
	m_pActiveFragmentShader = nullptr;
	m_pActiveRenderTarget = nullptr;

	CloseHandle(m_fenceEvent);

	DX_RELEASE(m_pCmdList);

	DX_RELEASE(m_pDepthStencilResource);
	DX_RELEASE(m_pHeapForDSV);

	for(uint32_t i = 0; i < kFrameBufferCount; ++i)
	{
		FrameBuffer& frameBuffer = m_frameBuffers[i];
		DX_RELEASE(frameBuffer.m_pRenderTarget);
		DX_RELEASE(frameBuffer.m_pCommandAllocator);
		DX_RELEASE(frameBuffer.m_pFence);
	};
	DX_RELEASE(m_pHeapForFrameBufferRTVs);

	DX_RELEASE(m_pSwapChain);
	DX_RELEASE(m_pCommandQueue);
	DX_RELEASE(m_pDevice);
}

void Direct3D12Render::AppendShaderFilenameWithPath(WritableString& outString, const String& shaderFilename) const
{
	outString += "data/shaders/hlsl/";
	outString += shaderFilename;
	outString += ".hlsl";
}

void Direct3D12Render::EndFrame()
{
	FrameBuffer& frameBuffer = m_frameBuffers[m_activeFrameBufferIdx];

	// Transition the render target from render target state to present state
	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(frameBuffer.m_pRenderTarget, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	m_pCmdList->ResourceBarrier(1, &barrier);

	HRESULT hr = m_pCmdList->Close();
	DX_CHECK_HRESULT(hr);

	ID3D12CommandList* commandLists[] = { m_pCmdList };
	m_pCommandQueue->ExecuteCommandLists(static_cast<UINT>(DESIRE_ASIZEOF(commandLists)), commandLists);

	// This command goes in at the end of our command queue.
	// We will know when our command queue has finished because the fence value will be set to "fenceValue" from the GPU
	hr = m_pCommandQueue->Signal(frameBuffer.m_pFence, frameBuffer.m_fenceValue);
	DX_CHECK_HRESULT(hr);

	hr = m_pSwapChain->Present(1, 0);
	DX_CHECK_HRESULT(hr);
}

void Direct3D12Render::Clear(uint32_t clearColorRGBA, float depth, uint8_t stencil)
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
		RenderTargetRenderDataD3D12* pRenderTargetRenderData = static_cast<RenderTargetRenderDataD3D12*>(m_pActiveRenderTarget->m_pRenderData);
		if(pRenderTargetRenderData != nullptr)
		{
			for(D3D12_CPU_DESCRIPTOR_HANDLE& renderTargetDescriptor : pRenderTargetRenderData->m_renderTargetDescriptors)
			{
				m_pCmdList->ClearRenderTargetView(renderTargetDescriptor, clearColor, 0, nullptr);
			}

			m_pCmdList->ClearDepthStencilView(pRenderTargetRenderData->m_depthStencilDescriptor, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, depth, stencil, 0, nullptr);
		}
	}
	else
	{
		FrameBuffer& frameBuffer = m_frameBuffers[m_activeFrameBufferIdx];
		m_pCmdList->ClearRenderTargetView(frameBuffer.m_RTV, clearColor, 0, nullptr);
		m_pCmdList->ClearDepthStencilView(m_pHeapForDSV->GetCPUDescriptorHandleForHeapStart(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, depth, stencil, 0, nullptr);
	}
}

void Direct3D12Render::SetWorldMatrix(const Matrix4& matrix)
{
	m_matWorld.r[0] = GetXMVECTOR(matrix.col0);
	m_matWorld.r[1] = GetXMVECTOR(matrix.col1);
	m_matWorld.r[2] = GetXMVECTOR(matrix.col2);
	m_matWorld.r[3] = GetXMVECTOR(matrix.col3);
}

void Direct3D12Render::SetViewProjectionMatrices(const Matrix4& viewMatrix, const Matrix4& projMatrix)
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

void Direct3D12Render::SetScissor(uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
	DESIRE_UNUSED(x);
	DESIRE_UNUSED(y);
	DESIRE_UNUSED(width);
	DESIRE_UNUSED(height);
}

void Direct3D12Render::SetColorWriteEnabled(bool r, bool g, bool b, bool a)
{
	ASSERT(!m_blendDesc.IndependentBlendEnable && "Independent render target blend states are not supported (only the RenderTarget[0] members are used)");

	if(r)
	{
		m_blendDesc.RenderTarget[0].RenderTargetWriteMask |= D3D12_COLOR_WRITE_ENABLE_RED;
	}
	else
	{
		m_blendDesc.RenderTarget[0].RenderTargetWriteMask &= ~D3D12_COLOR_WRITE_ENABLE_RED;
	}

	if(g)
	{
		m_blendDesc.RenderTarget[0].RenderTargetWriteMask |= D3D12_COLOR_WRITE_ENABLE_GREEN;
	}
	else
	{
		m_blendDesc.RenderTarget[0].RenderTargetWriteMask &= ~D3D12_COLOR_WRITE_ENABLE_GREEN;
	}

	if(b)
	{
		m_blendDesc.RenderTarget[0].RenderTargetWriteMask |= D3D12_COLOR_WRITE_ENABLE_BLUE;
	}
	else
	{
		m_blendDesc.RenderTarget[0].RenderTargetWriteMask &= ~D3D12_COLOR_WRITE_ENABLE_BLUE;
	}

	if(a)
	{
		m_blendDesc.RenderTarget[0].RenderTargetWriteMask |= D3D12_COLOR_WRITE_ENABLE_ALPHA;
	}
	else
	{
		m_blendDesc.RenderTarget[0].RenderTargetWriteMask &= ~D3D12_COLOR_WRITE_ENABLE_ALPHA;
	}
}

void Direct3D12Render::SetDepthWriteEnabled(bool enabled)
{
	m_depthStencilDesc.DepthWriteMask = enabled ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO;
}

void Direct3D12Render::SetDepthTest(EDepthTest depthTest)
{
	switch(depthTest)
	{
		case EDepthTest::Disabled:
			m_depthStencilDesc.DepthEnable = FALSE;
			return;

		case EDepthTest::Less:			m_depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS; break;
		case EDepthTest::LessEqual:		m_depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL; break;
		case EDepthTest::Greater:		m_depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_GREATER; break;
		case EDepthTest::GreaterEqual:	m_depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_GREATER_EQUAL; break;
		case EDepthTest::Equal:			m_depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_EQUAL; break;
		case EDepthTest::NotEqual:		m_depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_NOT_EQUAL; break;
	}

	m_depthStencilDesc.DepthEnable = TRUE;
}

void Direct3D12Render::SetCullMode(ECullMode cullMode)
{
	switch(cullMode)
	{
		case ECullMode::None:	m_rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE; break;
		case ECullMode::CCW:	m_rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK; break;
		case ECullMode::CW:		m_rasterizerDesc.CullMode = D3D12_CULL_MODE_FRONT; break;
	}
}

void Direct3D12Render::SetBlendModeSeparated(EBlend srcBlendRGB, EBlend destBlendRGB, EBlendOp blendOpRGB, EBlend srcBlendAlpha, EBlend destBlendAlpha, EBlendOp blendOpAlpha)
{
	ASSERT(!m_blendDesc.IndependentBlendEnable && "Independent render target blend states are not supported (only the RenderTarget[0] members are used)");

	m_blendDesc.RenderTarget[0].BlendEnable = TRUE;

	static constexpr D3D12_BLEND s_blendConversionTable[] =
	{
		D3D12_BLEND_ZERO,				// EBlend::Zero
		D3D12_BLEND_ONE,				// EBlend::One
		D3D12_BLEND_SRC_COLOR,			// EBlend::SrcColor
		D3D12_BLEND_INV_SRC_COLOR,		// EBlend::InvSrcColor
		D3D12_BLEND_SRC_ALPHA,			// EBlend::SrcAlpha
		D3D12_BLEND_INV_SRC_ALPHA,		// EBlend::InvSrcAlpha
		D3D12_BLEND_DEST_ALPHA,			// EBlend::DestAlpha
		D3D12_BLEND_INV_DEST_ALPHA,		// EBlend::InvDestAlpha
		D3D12_BLEND_DEST_COLOR,			// EBlend::DestColor
		D3D12_BLEND_INV_DEST_COLOR,		// EBlend::InvDestColor
		D3D12_BLEND_SRC_ALPHA_SAT,		// EBlend::SrcAlphaSat
		D3D12_BLEND_BLEND_FACTOR,		// EBlend::BlendFactor
		D3D12_BLEND_INV_BLEND_FACTOR	// EBlend::InvBlendFactor
	};
	DESIRE_CHECK_ARRAY_SIZE(s_blendConversionTable, EBlend::InvBlendFactor + 1);

	m_blendDesc.RenderTarget[0].SrcBlend = s_blendConversionTable[(size_t)srcBlendRGB];
	m_blendDesc.RenderTarget[0].DestBlend = s_blendConversionTable[(size_t)destBlendRGB];
	m_blendDesc.RenderTarget[0].SrcBlendAlpha = s_blendConversionTable[(size_t)srcBlendAlpha];
	m_blendDesc.RenderTarget[0].DestBlendAlpha = s_blendConversionTable[(size_t)destBlendAlpha];

	static constexpr D3D12_BLEND_OP s_equationConversionTable[] =
	{
		D3D12_BLEND_OP_ADD,				// EBlendOp::Add
		D3D12_BLEND_OP_SUBTRACT,		// EBlendOp::Subtract
		D3D12_BLEND_OP_REV_SUBTRACT,	// EBlendOp::RevSubtract
		D3D12_BLEND_OP_MIN,				// EBlendOp::Min
		D3D12_BLEND_OP_MAX				// EBlendOp::Max
	};
	DESIRE_CHECK_ARRAY_SIZE(s_equationConversionTable, EBlendOp::Max + 1);

	m_blendDesc.RenderTarget[0].BlendOp = s_equationConversionTable[(size_t)blendOpRGB];
	m_blendDesc.RenderTarget[0].BlendOpAlpha = s_equationConversionTable[(size_t)blendOpAlpha];
}

void Direct3D12Render::SetBlendModeDisabled()
{
	m_blendDesc.RenderTarget[0].BlendEnable = FALSE;
}

void* Direct3D12Render::CreateRenderableRenderData(const Renderable& renderable)
{
	DESIRE_UNUSED(renderable);
	return nullptr;
}

void* Direct3D12Render::CreateMeshRenderData(const Mesh& mesh)
{
	MeshRenderDataD3D12* pMeshRenderData = new MeshRenderDataD3D12();

	DESIRE_UNUSED(mesh);

	return pMeshRenderData;
}

void* Direct3D12Render::CreateShaderRenderData(const Shader& shader)
{
	ShaderRenderDataD3D12* pShaderRenderData = new ShaderRenderDataD3D12();

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

	ID3DBlob* pErrorBlob = nullptr;
	HRESULT hr = D3DCompile(shader.m_data.ptr.get(),		// pSrcData
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
			LOG_ERROR("Shader compile error:\n%s", static_cast<char*>(pErrorBlob->GetBufferPointer()));
			DX_RELEASE(pErrorBlob);
		}

		delete pShaderRenderData;
		return isVertexShader ? m_errorVertexShader->m_pRenderData : m_errorPixelShader->m_pRenderData;
	}

	ID3D12ShaderReflection* pReflection = nullptr;
	hr = D3DReflect(pShaderRenderData->m_pShaderCode->GetBufferPointer(), pShaderRenderData->m_pShaderCode->GetBufferSize(), IID_ID3D12ShaderReflection, (void**)&pReflection);
	if(FAILED(hr))
	{
		LOG_ERROR("D3DReflect failed 0x%08x\n", (uint32_t)hr);
	}

	D3D12_SHADER_DESC shaderDesc;
	hr = pReflection->GetDesc(&shaderDesc);
	if(FAILED(hr))
	{
		LOG_ERROR("ID3D12ShaderReflection::GetDesc failed 0x%08x\n", (uint32_t)hr);
	}

	pShaderRenderData->m_constantBuffers.SetSize(shaderDesc.ConstantBuffers);
	pShaderRenderData->m_constantBuffersData.SetSize(shaderDesc.ConstantBuffers);
	for(uint32_t i = 0; i < shaderDesc.ConstantBuffers; ++i)
	{
		ID3D12ShaderReflectionConstantBuffer* cbuffer = pReflection->GetConstantBufferByIndex(i);
		D3D12_SHADER_BUFFER_DESC shaderBufferDesc;
		hr = cbuffer->GetDesc(&shaderBufferDesc);
		DX_CHECK_HRESULT(hr);

/*		// Create constant buffer
		D3D12_BUFFER_DESC bufferDesc = {};
		bufferDesc.ByteWidth = shaderBufferDesc.Size;
		bufferDesc.Usage = D3D12_USAGE_DEFAULT;
		bufferDesc.BindFlags = D3D12_BIND_CONSTANT_BUFFER;
		hr = m_pDevice->CreateCommittedResource(&bufferDesc, nullptr, &pShaderRenderData->constantBuffers[i]);
		DX_CHECK_HRESULT(hr);

		// Create constant buffer data
		ShaderRenderDataD3D12::ConstantBufferData& bufferData = pShaderRenderData->constantBuffersData[i];
		bufferData.buffer = MemoryBuffer(shaderBufferDesc.Size);

		for(uint32_t j = 0; j < shaderBufferDesc.Variables; ++j)
		{
			ID3D12ShaderReflectionVariable* shaderVar = cbuffer->GetVariableByIndex(j);
			D3D12_SHADER_VARIABLE_DESC varDesc;
			hr = shaderVar->GetDesc(&varDesc);
			DX_CHECK_HRESULT(hr);

			if((varDesc.uFlags & D3D_SVF_USED) == 0)
			{
				continue;
			}

			ID3D12ShaderReflectionType* type = shaderVar->GetType();
			D3D12_SHADER_TYPE_DESC typeDesc;
			hr = type->GetDesc(&typeDesc);
			DX_CHECK_HRESULT(hr);

			if( typeDesc.Class != D3D_SVC_SCALAR &&
				typeDesc.Class != D3D_SVC_VECTOR &&
				typeDesc.Class != D3D_SVC_MATRIX_COLUMNS)
			{
				continue;
			}

			const HashedString key = HashedString::CreateFromString(String(varDesc.Name, strlen(varDesc.Name)));
			bufferData.m_variableOffsetSizePairs.Insert(key, std::pair(varDesc.StartOffset, varDesc.Size));
		}
*/
	}

	DX_RELEASE(pReflection);

	return pShaderRenderData;
}

void* Direct3D12Render::CreateTextureRenderData(const Texture& texture)
{
	TextureRenderDataD3D12* pTextureRenderData = new TextureRenderDataD3D12();

	D3D12_HEAP_PROPERTIES heapProperties = {};
	heapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
	heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapProperties.CreationNodeMask = 1;
	heapProperties.VisibleNodeMask = 1;

	D3D12_RESOURCE_DESC resourceDesc = {};
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resourceDesc.Width = texture.GetWidth();
	resourceDesc.Height = texture.GetHeight();
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = texture.GetNumMipLevels();
	resourceDesc.Format = GetTextureFormat(texture);
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	m_pDevice->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&pTextureRenderData->m_pTexture2D)
	);

	return pTextureRenderData;
}

void* Direct3D12Render::CreateRenderTargetRenderData(const RenderTarget& renderTarget)
{
	RenderTargetRenderDataD3D12* pRenderTargetRenderData = new RenderTargetRenderDataD3D12();

	DESIRE_UNUSED(renderTarget);

	return pRenderTargetRenderData;
}

void Direct3D12Render::DestroyRenderableRenderData(void* pRenderData)
{
	DESIRE_UNUSED(pRenderData);
	// No-op
}

void Direct3D12Render::DestroyMeshRenderData(void* pRenderData)
{
	MeshRenderDataD3D12* pMeshRenderData = static_cast<MeshRenderDataD3D12*>(pRenderData);
	delete pMeshRenderData;
}

void Direct3D12Render::DestroyShaderRenderData(void* pRenderData)
{
	ShaderRenderDataD3D12* pShaderRenderData = static_cast<ShaderRenderDataD3D12*>(pRenderData);

	if((pShaderRenderData == m_errorVertexShader->m_pRenderData || pShaderRenderData == m_errorPixelShader->m_pRenderData)
//		&& pShader != errorVertexshader.m_get()
//		&& pShader != errorPixelshader.m_get()
		)
	{
		return;
	}

	delete pShaderRenderData;
}

void Direct3D12Render::DestroyTextureRenderData(void* pRenderData)
{
	TextureRenderDataD3D12* pTextureRenderData = static_cast<TextureRenderDataD3D12*>(pRenderData);
	delete pTextureRenderData;
}

void Direct3D12Render::DestroyRenderTargetRenderData(void* pRenderData)
{
	RenderTargetRenderDataD3D12* pRenderTargetRenderData = static_cast<RenderTargetRenderDataD3D12*>(pRenderData);
	delete pRenderTargetRenderData;
}

void Direct3D12Render::SetMesh(Mesh& mesh)
{
	MeshRenderDataD3D12* pMeshRenderData = static_cast<MeshRenderDataD3D12*>(mesh.m_pRenderData);
	m_pCmdList->IASetIndexBuffer(&pMeshRenderData->indexBufferView);
	m_pCmdList->IASetVertexBuffers(0, 1, &pMeshRenderData->vertexBufferView);
}

void Direct3D12Render::UpdateDynamicMesh(DynamicMesh& dynamicMesh)
{
	if(dynamicMesh.m_isIndicesDirty)
	{
		dynamicMesh.m_isIndicesDirty = false;
	}

	if(dynamicMesh.m_isVerticesDirty)
	{
		dynamicMesh.m_isVerticesDirty = false;
	}
}

void Direct3D12Render::SetVertexShader(Shader& vertexShader)
{
	const ShaderRenderDataD3D12* pShaderRenderData = static_cast<const ShaderRenderDataD3D12*>(vertexShader.m_pRenderData);
	DESIRE_UNUSED(pShaderRenderData);
}

void Direct3D12Render::SetFragmentShader(Shader& fragmentShader)
{
	const ShaderRenderDataD3D12* pShaderRenderData = static_cast<const ShaderRenderDataD3D12*>(fragmentShader.m_pRenderData);
	DESIRE_UNUSED(pShaderRenderData);
}

void Direct3D12Render::SetTexture(uint8_t samplerIdx, const Texture& texture, EFilterMode filterMode, EAddressMode addressMode)
{
	DESIRE_UNUSED(samplerIdx);

	ASSERT(samplerIdx < D3D12_COMMONSHADER_SAMPLER_SLOT_COUNT);

//	const TextureRenderDataD3D12* pTextureRenderData = static_cast<TextureRenderDataD3D12*>(texture->m_pRenderData);

	static constexpr D3D12_TEXTURE_ADDRESS_MODE s_addressModeConversionTable[] =
	{
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,		// ETextureWrapMode::Repeat
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,		// ETextureWrapMode::Clamp
		D3D12_TEXTURE_ADDRESS_MODE_MIRROR,		// ETextureWrapMode::MirroredRepeat
		D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE,	// ETextureWrapMode::MirrorOnce
		D3D12_TEXTURE_ADDRESS_MODE_BORDER,		// ETextureWrapMode::Border
	};

	D3D12_SAMPLER_DESC samplerDesc = {};
	switch(filterMode)
	{
		case EFilterMode::Point:		samplerDesc.Filter = D3D12_ENCODE_BASIC_FILTER(D3D12_FILTER_TYPE_POINT, D3D12_FILTER_TYPE_POINT, D3D12_FILTER_TYPE_POINT, D3D12_FILTER_REDUCTION_TYPE_STANDARD); break;
		case EFilterMode::Bilinear:		samplerDesc.Filter = D3D12_ENCODE_BASIC_FILTER(D3D12_FILTER_TYPE_LINEAR, D3D12_FILTER_TYPE_LINEAR, D3D12_FILTER_TYPE_POINT, D3D12_FILTER_REDUCTION_TYPE_STANDARD); break;
		case EFilterMode::Trilinear:	samplerDesc.Filter = D3D12_ENCODE_BASIC_FILTER(D3D12_FILTER_TYPE_LINEAR, D3D12_FILTER_TYPE_LINEAR, D3D12_FILTER_TYPE_LINEAR, D3D12_FILTER_REDUCTION_TYPE_STANDARD); break;
		case EFilterMode::Anisotropic:	samplerDesc.Filter = D3D12_ENCODE_ANISOTROPIC_FILTER(D3D12_FILTER_REDUCTION_TYPE_STANDARD); break;
	}

	samplerDesc.AddressU = s_addressModeConversionTable[(size_t)addressMode];
	samplerDesc.AddressV = s_addressModeConversionTable[(size_t)addressMode];
	samplerDesc.AddressW = s_addressModeConversionTable[(size_t)addressMode];
	samplerDesc.MaxAnisotropy = D3D12_MAX_MAXANISOTROPY;
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;

	DESIRE_UNUSED(texture);
}

void Direct3D12Render::SetRenderTarget(RenderTarget* pRenderTarget)
{
	D3D12_VIEWPORT viewport = { 0.0f, 0.0f, static_cast<FLOAT>(m_pActiveWindow->GetWidth()), static_cast<FLOAT>(m_pActiveWindow->GetHeight()), 0.0f, 1.0f };

	if(pRenderTarget != nullptr)
	{
		const RenderTargetRenderDataD3D12* pRenderTargetRenderData = static_cast<RenderTargetRenderDataD3D12*>(pRenderTarget->m_pRenderData);
		m_pCmdList->OMSetRenderTargets(pRenderTargetRenderData->m_numRenderTargetDescriptors, pRenderTargetRenderData->m_renderTargetDescriptors, TRUE, &pRenderTargetRenderData->m_depthStencilDescriptor);

		viewport.Width = static_cast<FLOAT>(pRenderTarget->GetWidth());
		viewport.Height = static_cast<FLOAT>(pRenderTarget->GetHeight());
	}
	else
	{
		// TODO: move this to BeginFrame
		{
			// We have to wait for the gpu to finish with the command allocator before we reset it
			WaitForPreviousFrame();

			FrameBuffer& frameBuffer = m_frameBuffers[m_activeFrameBufferIdx];
			HRESULT hr = frameBuffer.m_pCommandAllocator->Reset();
			DX_CHECK_HRESULT(hr);

			// By resetting the command list we are putting it into a recording state so we can start recording commands
			ID3D12PipelineState* pPipelineState = nullptr;
			hr = m_pCmdList->Reset(frameBuffer.m_pCommandAllocator, pPipelineState);

			// Transition the render target from present state to render target state
			CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(frameBuffer.m_pRenderTarget, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
			m_pCmdList->ResourceBarrier(1, &barrier);
		}

		FrameBuffer& frameBuffer = m_frameBuffers[m_activeFrameBufferIdx];
		const D3D12_CPU_DESCRIPTOR_HANDLE frameBufferDSV = m_pHeapForDSV->GetCPUDescriptorHandleForHeapStart();
		m_pCmdList->OMSetRenderTargets(1, &frameBuffer.m_RTV, TRUE, &frameBufferDSV);
	}

	m_pCmdList->RSSetViewports(1, &viewport);
}

void Direct3D12Render::UpdateShaderParams(const Material& material)
{
	const ShaderRenderDataD3D12* pVertexShaderRenderData = static_cast<const ShaderRenderDataD3D12*>(m_pActiveVertexShader->m_pRenderData);
	UpdateShaderParams(material, pVertexShaderRenderData);

	const ShaderRenderDataD3D12* pFragmentShaderRenderData = static_cast<const ShaderRenderDataD3D12*>(m_pActiveFragmentShader->m_pRenderData);
	UpdateShaderParams(material, pFragmentShaderRenderData);
}

void Direct3D12Render::UpdateShaderParams(const Material& material, const ShaderRenderDataD3D12* pShaderRenderData)
{
	const std::pair<uint32_t, uint32_t>* pOffsetSizePair = nullptr;

	for(size_t i = 0; i < pShaderRenderData->m_constantBuffers.Size(); ++i)
	{
		bool isChanged = false;
		const ShaderRenderDataD3D12::ConstantBufferData& bufferData = pShaderRenderData->m_constantBuffersData[i];

		for(const Material::ShaderParam& shaderParam : material.GetShaderParams())
		{
			pOffsetSizePair = bufferData.m_variableOffsetSizePairs.Find(shaderParam.m_name);
			if(pOffsetSizePair != nullptr)
			{
				isChanged |= CheckAndUpdateShaderParam(shaderParam.GetValue(), bufferData.m_data.ptr.get() + pOffsetSizePair->first, pOffsetSizePair->second);
			}
		}

		pOffsetSizePair = bufferData.m_variableOffsetSizePairs.Find("matWorldView");
		if(pOffsetSizePair != nullptr && pOffsetSizePair->second == sizeof(DirectX::XMMATRIX))
		{
			const DirectX::XMMATRIX matWorldView = DirectX::XMMatrixMultiply(m_matWorld, m_matView);
			isChanged |= CheckAndUpdateShaderParam(&matWorldView.r[0], bufferData.m_data.ptr.get() + pOffsetSizePair->first, pOffsetSizePair->second);
		}

		pOffsetSizePair = bufferData.m_variableOffsetSizePairs.Find("matWorldViewProj");
		if(pOffsetSizePair != nullptr && pOffsetSizePair->second == sizeof(DirectX::XMMATRIX))
		{
			const DirectX::XMMATRIX matWorldView = DirectX::XMMatrixMultiply(m_matWorld, m_matView);
			const DirectX::XMMATRIX matWorldViewProj = DirectX::XMMatrixMultiply(matWorldView, m_matProj);
			isChanged |= CheckAndUpdateShaderParam(&matWorldViewProj.r[0], bufferData.m_data.ptr.get() + pOffsetSizePair->first, pOffsetSizePair->second);
		}

		pOffsetSizePair = bufferData.m_variableOffsetSizePairs.Find("matView");
		if(pOffsetSizePair != nullptr)
		{
			isChanged |= CheckAndUpdateShaderParam(&m_matView.r[0], bufferData.m_data.ptr.get() + pOffsetSizePair->first, pOffsetSizePair->second);
		}

		pOffsetSizePair = bufferData.m_variableOffsetSizePairs.Find("matViewInv");
		if(pOffsetSizePair != nullptr)
		{
			const DirectX::XMMATRIX matViewInv = DirectX::XMMatrixInverse(nullptr, m_matView);
			isChanged |= CheckAndUpdateShaderParam(&matViewInv.r[0], bufferData.m_data.ptr.get() + pOffsetSizePair->first, pOffsetSizePair->second);
		}

		pOffsetSizePair = bufferData.m_variableOffsetSizePairs.Find("camPos");
		if(pOffsetSizePair != nullptr)
		{
			const DirectX::XMMATRIX matViewInv = DirectX::XMMatrixInverse(nullptr, m_matView);
			isChanged |= CheckAndUpdateShaderParam(&matViewInv.r[3], bufferData.m_data.ptr.get() + pOffsetSizePair->first, pOffsetSizePair->second);
		}

		pOffsetSizePair = bufferData.m_variableOffsetSizePairs.Find("resolution");
		if(pOffsetSizePair != nullptr && pOffsetSizePair->second == 2 * sizeof(float))
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

			isChanged |= CheckAndUpdateShaderParam(resolution, bufferData.m_data.ptr.get() + pOffsetSizePair->first, pOffsetSizePair->second);
		}

		if(isChanged)
		{
//			deviceCtx->UpdateSubresource(pShaderRenderData->constantBuffers[i], 0, nullptr, bufferData.m_data.ptr.get(), 0, 0);
		}
	}
}

bool Direct3D12Render::CheckAndUpdateShaderParam(const void* value, void* valueInConstantBuffer, uint32_t size)
{
	if(memcmp(valueInConstantBuffer, value, size) == 0)
	{
		return false;
	}

	memcpy(valueInConstantBuffer, value, size);
	return true;
}

void Direct3D12Render::DoRender(Renderable& renderable, uint32_t indexOffset, uint32_t vertexOffset, uint32_t numIndices, uint32_t numVertices)
{
	DESIRE_UNUSED(renderable);
	DESIRE_UNUSED(indexOffset);
	DESIRE_UNUSED(vertexOffset);
	DESIRE_UNUSED(numVertices);

	if(numIndices != 0)
	{
	}
	else
	{
	}
}

bool Direct3D12Render::CreateFrameBuffers(uint32_t width, uint32_t height)
{
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.NumDescriptors = kFrameBufferCount;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	HRESULT hr = m_pDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_pHeapForFrameBufferRTVs));
	if(FAILED(hr))
	{
		return false;
	}
//	m_pHeapForFrameBufferRTVs->SetName(L"m_pHeapForFrameBufferRTVs");

	const UINT descriptorHandleIncrementSize = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	D3D12_CPU_DESCRIPTOR_HANDLE currRTV = m_pHeapForFrameBufferRTVs->GetCPUDescriptorHandleForHeapStart();
	for(uint32_t i = 0; i < kFrameBufferCount; ++i)
	{
		FrameBuffer& frameBuffer = m_frameBuffers[i];
		frameBuffer.m_RTV = currRTV;
		currRTV.ptr += descriptorHandleIncrementSize;

		hr = m_pSwapChain->GetBuffer(i, IID_PPV_ARGS(&frameBuffer.m_pRenderTarget));
		DX_CHECK_HRESULT(hr);
		m_pDevice->CreateRenderTargetView(frameBuffer.m_pRenderTarget, nullptr, frameBuffer.m_RTV);

		hr = m_pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&frameBuffer.m_pCommandAllocator));
		if(FAILED(hr))
		{
			return false;
		}

		frameBuffer.m_fenceValue = 0;
		hr = m_pDevice->CreateFence(frameBuffer.m_fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&frameBuffer.m_pFence));
		if(FAILED(hr))
		{
			return false;
		}
	}

	// Create the depth/stencil buffer
	CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_DEFAULT);
	CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, width, height);
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
	depthOptimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
	depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
	depthOptimizedClearValue.DepthStencil.Stencil = 0;

	hr = m_pDevice->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&depthOptimizedClearValue,
		IID_PPV_ARGS(&m_pDepthStencilResource)
	);
	if(FAILED(hr))
	{
		return false;
	}

	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	hr = m_pDevice->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&m_pHeapForDSV));
	if(FAILED(hr))
	{
		return false;
	}
//	m_pHeapForDSV->SetName(L"m_pHeapForDSV");

	D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
	depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthStencilViewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Flags = D3D12_DSV_FLAG_NONE;

	m_pDevice->CreateDepthStencilView(m_pDepthStencilResource, &depthStencilViewDesc, m_pHeapForDSV->GetCPUDescriptorHandleForHeapStart());

	return true;
}

void Direct3D12Render::WaitForPreviousFrame()
{
	FrameBuffer& frameBuffer = m_frameBuffers[m_activeFrameBufferIdx];
	if(frameBuffer.m_pFence->GetCompletedValue() < frameBuffer.m_fenceValue)
	{
		// The GPU has not finished executing the command queue so we will wait until the fence has triggered the event
		HRESULT hr = frameBuffer.m_pFence->SetEventOnCompletion(frameBuffer.m_fenceValue, m_fenceEvent);
		DX_CHECK_HRESULT(hr);
		WaitForSingleObject(m_fenceEvent, INFINITE);
	}

	frameBuffer.m_fenceValue++;

	m_activeFrameBufferIdx = m_pSwapChain->GetCurrentBackBufferIndex();
}

DXGI_FORMAT Direct3D12Render::GetTextureFormat(const Texture& texture)
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
