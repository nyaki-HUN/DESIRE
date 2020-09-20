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
	errorVertexShader = std::make_unique<Shader>("vs_error");
	errorVertexShader->data = MemoryBuffer::CreateFromDataCopy(vs_error, sizeof(vs_error));
	errorPixelShader = std::make_unique<Shader>("ps_error");
	errorPixelShader->data = MemoryBuffer::CreateFromDataCopy(ps_error, sizeof(ps_error));

	// Stencil test parameters
	depthStencilDesc.StencilEnable = FALSE;
	depthStencilDesc.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
	depthStencilDesc.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;
	// Stencil operations if pixel is front-facing
	depthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	// Stencil operations if pixel is back-facing
	depthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;

	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
	rasterizerDesc.DepthClipEnable = TRUE;
	rasterizerDesc.MultisampleEnable = TRUE;
	rasterizerDesc.AntialiasedLineEnable = TRUE;

	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	matWorld = DirectX::XMMatrixIdentity();
	matView = DirectX::XMMatrixIdentity();
	matProj = DirectX::XMMatrixIdentity();
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
		pDebugController->Release();
	}
#endif

	IDXGIFactory4* pFactory = nullptr;
	hr = CreateDXGIFactory1(IID_PPV_ARGS(&pFactory));
	if(FAILED(hr))
	{
		return false;
	}

	// Create device
	hr = D3D12CreateDevice(
		nullptr,
		D3D_FEATURE_LEVEL_12_1,
		IID_PPV_ARGS(&pDevice)
	);

	if(FAILED(hr))
	{
		return false;
	}

	// Create command queue
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc = {};
	commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	commandQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

	hr = pDevice->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&pCommandQueue));
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
	hr = pFactory->CreateSwapChainForHwnd(pCommandQueue, static_cast<HWND>(mainWindow.GetHandle()), &swapChainDesc, nullptr, nullptr, &pTmpSwapChain);
	if(FAILED(hr))
	{
		return false;
	}

	hr = pTmpSwapChain->QueryInterface(IID_PPV_ARGS(&pSwapChain));
	DX_RELEASE(pTmpSwapChain);
	if(FAILED(hr))
	{
		return false;
	}

	// Create the frame buffers
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.NumDescriptors = kFrameBufferCount;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	hr = pDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&pFrameBufferRenderTargetDescriptorHeap));
	if(FAILED(hr))
	{
		return false;
	}
	pFrameBufferRenderTargetDescriptorHeap->SetName(L"pFrameBufferRenderTargetDescriptorHeap");

	const UINT descriptorHandleIncrementSize = pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	D3D12_CPU_DESCRIPTOR_HANDLE handleRTV = pFrameBufferRenderTargetDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	for(uint32_t i = 0; i < kFrameBufferCount; ++i)
	{
		FrameBuffer& frameBuffer = frameBuffers[i];
		frameBuffer.renderTargetDescriptor = handleRTV;
		handleRTV.ptr += descriptorHandleIncrementSize;

		hr = pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&frameBuffer.pCommandAllocator));
		if(FAILED(hr))
		{
			return false;
		}

		hr = pDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&frameBuffer.pFence));
		if(FAILED(hr))
		{
			return false;
		}

		frameBuffer.fenceValue = 0;
	}

	// Create the depth/stencil buffer
	D3D12_HEAP_PROPERTIES heapProperties = {};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
	heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapProperties.CreationNodeMask = 1;
	heapProperties.VisibleNodeMask = 1;

	D3D12_RESOURCE_DESC resourceDesc = {};
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resourceDesc.Width = mainWindow.GetWidth();
	resourceDesc.Height = mainWindow.GetHeight();
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 0;
	resourceDesc.Format = DXGI_FORMAT_D32_FLOAT;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
	depthOptimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
	depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
	depthOptimizedClearValue.DepthStencil.Stencil = 0;

	hr = pDevice->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&depthOptimizedClearValue,
		IID_PPV_ARGS(&pDepthStencilResource)
	);
	if(FAILED(hr))
	{
		return false;
	}

	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	hr = pDevice->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&pDepthStencilDescriptorHeap));
	if(FAILED(hr))
	{
		return false;
	}
	pDepthStencilDescriptorHeap->SetName(L"pDepthStencilDescriptorHeap");

	D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
	depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthStencilViewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Flags = D3D12_DSV_FLAG_NONE;

	pDevice->CreateDepthStencilView(pDepthStencilResource, &depthStencilViewDesc, pDepthStencilDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

	// Create command list
	hr = pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, frameBuffers[0].pCommandAllocator, nullptr, IID_PPV_ARGS(&pCmdList));
	if(FAILED(hr))
	{
		return false;
	}

	// Command lists are created in the recording state, but we will set it up for recording again so close it now
	hr = pCmdList->Close();
	if(FAILED(hr))
	{
		return false;
	}

	fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if(fenceEvent == nullptr)
	{
		return false;
	}

	activeFrameBufferIdx = pSwapChain->GetCurrentBackBufferIndex();

	CreateFrameBuffers();

	Bind(*errorVertexShader);
	Bind(*errorPixelShader);

	return true;
}

void Direct3D12Render::UpdateRenderWindow(OSWindow& window)
{
	HRESULT hr = pSwapChain->ResizeBuffers(0, window.GetWidth(), window.GetHeight(), DXGI_FORMAT_UNKNOWN, 0);
	DX_CHECK_HRESULT(hr);

	WaitForPreviousFrame();

	for(uint32_t i = 0; i < kFrameBufferCount; ++i)
	{
		DX_RELEASE(frameBuffers[i].pRenderTargetResource);
	}

	CreateFrameBuffers();
}

void Direct3D12Render::Kill()
{
	// Wait for the GPU to finish all frames
	for(uint32_t i = 0; i < kFrameBufferCount; ++i)
	{
		activeFrameBufferIdx = i;
		WaitForPreviousFrame();
	}

	Unbind(*errorVertexShader);
	Unbind(*errorPixelShader);

	pActiveWindow = nullptr;
	pActiveMesh = nullptr;
	pActiveVertexShader = nullptr;
	pActiveFragmentShader = nullptr;
	pActiveRenderTarget = nullptr;

	CloseHandle(fenceEvent);

	DX_RELEASE(pCmdList);

	DX_RELEASE(pDepthStencilResource);
	DX_RELEASE(pDepthStencilDescriptorHeap);

	for(uint32_t i = 0; i < kFrameBufferCount; ++i)
	{
		FrameBuffer& frameBuffer = frameBuffers[i];
		DX_RELEASE(frameBuffer.pRenderTargetResource);
		DX_RELEASE(frameBuffer.pCommandAllocator);
		DX_RELEASE(frameBuffer.pFence);
	};
	DX_RELEASE(pFrameBufferRenderTargetDescriptorHeap);

	DX_RELEASE(pSwapChain);
	DX_RELEASE(pCommandQueue);
	DX_RELEASE(pDevice);
}

void Direct3D12Render::AppendShaderFilenameWithPath(WritableString& outString, const String& shaderFilename) const
{
	outString += "data/shaders/hlsl/";
	outString += shaderFilename;
	outString += ".hlsl";
}

void Direct3D12Render::EndFrame()
{
	FrameBuffer& frameBuffer = frameBuffers[activeFrameBufferIdx];

	// Transition the render target from render target state to present state
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = frameBuffer.pRenderTargetResource;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	pCmdList->ResourceBarrier(1, &barrier);

	HRESULT hr = pCmdList->Close();
	DX_CHECK_HRESULT(hr);

	ID3D12CommandList* commandLists[] = { pCmdList };
	pCommandQueue->ExecuteCommandLists(static_cast<UINT>(DESIRE_ASIZEOF(commandLists)), commandLists);

	// This command goes in at the end of our command queue.
	// We will know when our command queue has finished because the fence value will be set to "fenceValue" from the GPU
	hr = pCommandQueue->Signal(frameBuffer.pFence, frameBuffer.fenceValue);
	DX_CHECK_HRESULT(hr);

	hr = pSwapChain->Present(1, 0);
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

	if(pActiveRenderTarget != nullptr)
	{
		RenderTargetRenderDataD3D12* pRenderTargetRenderData = static_cast<RenderTargetRenderDataD3D12*>(pActiveRenderTarget->pRenderData);
		if(pRenderTargetRenderData != nullptr)
		{
			for(D3D12_CPU_DESCRIPTOR_HANDLE& renderTargetDescriptor : pRenderTargetRenderData->renderTargetDescriptors)
			{
				pCmdList->ClearRenderTargetView(renderTargetDescriptor, clearColor, 0, nullptr);
			}

			pCmdList->ClearDepthStencilView(pRenderTargetRenderData->depthStencilDescriptor, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, depth, stencil, 0, nullptr);
		}
	}
	else
	{
		FrameBuffer& frameBuffer = frameBuffers[activeFrameBufferIdx];
		pCmdList->ClearRenderTargetView(frameBuffer.renderTargetDescriptor, clearColor, 0, nullptr);
		pCmdList->ClearDepthStencilView(pDepthStencilDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, depth, stencil, 0, nullptr);
	}
}

void Direct3D12Render::SetWorldMatrix(const Matrix4& matrix)
{
	matWorld.r[0] = GetXMVECTOR(matrix.col0);
	matWorld.r[1] = GetXMVECTOR(matrix.col1);
	matWorld.r[2] = GetXMVECTOR(matrix.col2);
	matWorld.r[3] = GetXMVECTOR(matrix.col3);
}

void Direct3D12Render::SetViewProjectionMatrices(const Matrix4& viewMatrix, const Matrix4& projMatrix)
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

void Direct3D12Render::SetScissor(uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
	DESIRE_UNUSED(x);
	DESIRE_UNUSED(y);
	DESIRE_UNUSED(width);
	DESIRE_UNUSED(height);
}

void Direct3D12Render::SetColorWriteEnabled(bool r, bool g, bool b, bool a)
{
	ASSERT(!blendDesc.IndependentBlendEnable && "Independent render target blend states are not supported (only the RenderTarget[0] members are used)");

	if(r)
	{
		blendDesc.RenderTarget[0].RenderTargetWriteMask |= D3D12_COLOR_WRITE_ENABLE_RED;
	}
	else
	{
		blendDesc.RenderTarget[0].RenderTargetWriteMask &= ~D3D12_COLOR_WRITE_ENABLE_RED;
	}

	if(g)
	{
		blendDesc.RenderTarget[0].RenderTargetWriteMask |= D3D12_COLOR_WRITE_ENABLE_GREEN;
	}
	else
	{
		blendDesc.RenderTarget[0].RenderTargetWriteMask &= ~D3D12_COLOR_WRITE_ENABLE_GREEN;
	}

	if(b)
	{
		blendDesc.RenderTarget[0].RenderTargetWriteMask |= D3D12_COLOR_WRITE_ENABLE_BLUE;
	}
	else
	{
		blendDesc.RenderTarget[0].RenderTargetWriteMask &= ~D3D12_COLOR_WRITE_ENABLE_BLUE;
	}

	if(a)
	{
		blendDesc.RenderTarget[0].RenderTargetWriteMask |= D3D12_COLOR_WRITE_ENABLE_ALPHA;
	}
	else
	{
		blendDesc.RenderTarget[0].RenderTargetWriteMask &= ~D3D12_COLOR_WRITE_ENABLE_ALPHA;
	}
}

void Direct3D12Render::SetDepthWriteEnabled(bool enabled)
{
	depthStencilDesc.DepthWriteMask = enabled ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO;
}

void Direct3D12Render::SetDepthTest(EDepthTest depthTest)
{
	switch(depthTest)
	{
		case EDepthTest::Disabled:
			depthStencilDesc.DepthEnable = FALSE;
			return;

		case EDepthTest::Less:			depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS; break;
		case EDepthTest::LessEqual:		depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL; break;
		case EDepthTest::Greater:		depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_GREATER; break;
		case EDepthTest::GreaterEqual:	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_GREATER_EQUAL; break;
		case EDepthTest::Equal:			depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_EQUAL; break;
		case EDepthTest::NotEqual:		depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_NOT_EQUAL; break;
	}

	depthStencilDesc.DepthEnable = TRUE;
}

void Direct3D12Render::SetCullMode(ECullMode cullMode)
{
	switch(cullMode)
	{
		case ECullMode::None:	rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE; break;
		case ECullMode::CCW:	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK; break;
		case ECullMode::CW:		rasterizerDesc.CullMode = D3D12_CULL_MODE_FRONT; break;
	}
}

void Direct3D12Render::SetBlendModeSeparated(EBlend srcBlendRGB, EBlend destBlendRGB, EBlendOp blendOpRGB, EBlend srcBlendAlpha, EBlend destBlendAlpha, EBlendOp blendOpAlpha)
{
	ASSERT(!blendDesc.IndependentBlendEnable && "Independent render target blend states are not supported (only the RenderTarget[0] members are used)");

	blendDesc.RenderTarget[0].BlendEnable = TRUE;

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

	blendDesc.RenderTarget[0].SrcBlend = s_blendConversionTable[(size_t)srcBlendRGB];
	blendDesc.RenderTarget[0].DestBlend = s_blendConversionTable[(size_t)destBlendRGB];
	blendDesc.RenderTarget[0].SrcBlendAlpha = s_blendConversionTable[(size_t)srcBlendAlpha];
	blendDesc.RenderTarget[0].DestBlendAlpha = s_blendConversionTable[(size_t)destBlendAlpha];

	static constexpr D3D12_BLEND_OP s_equationConversionTable[] =
	{
		D3D12_BLEND_OP_ADD,				// EBlendOp::Add
		D3D12_BLEND_OP_SUBTRACT,		// EBlendOp::Subtract
		D3D12_BLEND_OP_REV_SUBTRACT,	// EBlendOp::RevSubtract
		D3D12_BLEND_OP_MIN,				// EBlendOp::Min
		D3D12_BLEND_OP_MAX				// EBlendOp::Max
	};
	DESIRE_CHECK_ARRAY_SIZE(s_equationConversionTable, EBlendOp::Max + 1);

	blendDesc.RenderTarget[0].BlendOp = s_equationConversionTable[(size_t)blendOpRGB];
	blendDesc.RenderTarget[0].BlendOpAlpha = s_equationConversionTable[(size_t)blendOpAlpha];
}

void Direct3D12Render::SetBlendModeDisabled()
{
	blendDesc.RenderTarget[0].BlendEnable = FALSE;
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
	ASSERT(shader.defines.size() < DESIRE_ASIZEOF(defines));
	D3D_SHADER_MACRO* definePtr = &defines[0];
	for(const String& define : shader.defines)
	{
		definePtr->Name = define.Str();
		definePtr->Definition = "1";
		definePtr++;
	}

	StackString<DESIRE_MAX_PATH_LEN> filenameWithPath = FileSystem::Get()->GetAppDirectory();
	AppendShaderFilenameWithPath(filenameWithPath, shader.name);

	const bool isVertexShader = shader.name.StartsWith("vs_");
	UINT compileFlags = 0;

	ID3DBlob* pErrorBlob = nullptr;
	HRESULT hr = D3DCompile(shader.data.ptr.get(),		// pSrcData
		shader.data.size,								// SrcDataSize
		filenameWithPath.Str(),							// pSourceName
		defines,										// pDefines
		D3D_COMPILE_STANDARD_FILE_INCLUDE,				// pInclude
		"main",											// pEntrypoint
		isVertexShader ? "vs_5_0" : "ps_5_0",			// pTarget
		compileFlags,									// D3DCOMPILE flags
		0,												// D3DCOMPILE_EFFECT flags
		&pShaderRenderData->shaderCode,					// ppCode
		&pErrorBlob);									// ppErrorMsgs
	if(FAILED(hr))
	{
		if(pErrorBlob != nullptr)
		{
			LOG_ERROR("Shader compile error:\n%s", static_cast<char*>(pErrorBlob->GetBufferPointer()));
			DX_RELEASE(pErrorBlob);
		}

		delete pShaderRenderData;
		return isVertexShader ? errorVertexShader->pRenderData : errorPixelShader->pRenderData;
	}

	if(isVertexShader)
	{
//		hr = pDevice->CreateVertexShader(pShaderRenderData->shaderCode->GetBufferPointer(), pShaderRenderData->shaderCode->GetBufferSize(), nullptr, &pShaderRenderData->vertexShader);
	}
	else
	{
//		hr = pDevice->CreatePixelShader(pShaderRenderData->shaderCode->GetBufferPointer(), pShaderRenderData->shaderCode->GetBufferSize(), nullptr, &pShaderRenderData->pixelShader);
	}
	DX_CHECK_HRESULT(hr);

//	pShaderRenderData->ptr->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(shader.name.Length()), shader.name.Str());

	ID3D12ShaderReflection* pReflection = nullptr;
	hr = D3DReflect(pShaderRenderData->shaderCode->GetBufferPointer(), pShaderRenderData->shaderCode->GetBufferSize(), IID_ID3D12ShaderReflection, (void**)&pReflection);
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

	pShaderRenderData->constantBuffers.SetSize(shaderDesc.ConstantBuffers);
	pShaderRenderData->constantBuffersData.SetSize(shaderDesc.ConstantBuffers);
/*	for(uint32_t i = 0; i < shaderDesc.ConstantBuffers; ++i)
	{
		ID3D12ShaderReflectionConstantBuffer* cbuffer = pReflection->GetConstantBufferByIndex(i);
		D3D12_SHADER_BUFFER_DESC shaderBufferDesc;
		hr = cbuffer->GetDesc(&shaderBufferDesc);
		DX_CHECK_HRESULT(hr);

		// Create constant buffer
		D3D12_BUFFER_DESC bufferDesc = {};
		bufferDesc.ByteWidth = shaderBufferDesc.Size;
		bufferDesc.Usage = D3D12_USAGE_DEFAULT;
		bufferDesc.BindFlags = D3D12_BIND_CONSTANT_BUFFER;
		hr = pDevice->CreateCommittedResource(&bufferDesc, nullptr, &pShaderRenderData->constantBuffers[i]);
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
			bufferData.variableOffsetSizePairs.Insert(key, std::pair(varDesc.StartOffset, varDesc.Size));
		}
	}
*/

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

	pDevice->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&pTextureRenderData->pTexture2D)
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

	if((pShaderRenderData == errorVertexShader->pRenderData || pShaderRenderData == errorPixelShader->pRenderData)
//		&& pShader != errorVertexShader.get()
//		&& pShader != errorPixelShader.get()
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
	MeshRenderDataD3D12* pMeshRenderData = static_cast<MeshRenderDataD3D12*>(mesh.pRenderData);
	pCmdList->IASetIndexBuffer(&pMeshRenderData->indexBufferView);
	pCmdList->IASetVertexBuffers(0, 1, &pMeshRenderData->vertexBufferView);
}

void Direct3D12Render::UpdateDynamicMesh(DynamicMesh& dynamicMesh)
{
	if(dynamicMesh.isIndicesDirty)
	{
		dynamicMesh.isIndicesDirty = false;
	}

	if(dynamicMesh.isVerticesDirty)
	{
		dynamicMesh.isVerticesDirty = false;
	}
}

void Direct3D12Render::SetVertexShader(Shader& vertexShader)
{
	const ShaderRenderDataD3D12* pShaderRenderData = static_cast<const ShaderRenderDataD3D12*>(vertexShader.pRenderData);
	DESIRE_UNUSED(pShaderRenderData);
}

void Direct3D12Render::SetFragmentShader(Shader& fragmentShader)
{
	const ShaderRenderDataD3D12* pShaderRenderData = static_cast<const ShaderRenderDataD3D12*>(fragmentShader.pRenderData);
	DESIRE_UNUSED(pShaderRenderData);
}

void Direct3D12Render::SetTexture(uint8_t samplerIdx, const Texture& texture, EFilterMode filterMode, EAddressMode addressMode)
{
	DESIRE_UNUSED(samplerIdx);

	ASSERT(samplerIdx < D3D12_COMMONSHADER_SAMPLER_SLOT_COUNT);

//	const TextureRenderDataD3D12* pTextureRenderData = static_cast<TextureRenderDataD3D12*>(texture->pRenderData);

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
	D3D12_VIEWPORT viewport = { 0.0f, 0.0f, static_cast<FLOAT>(pActiveWindow->GetWidth()), static_cast<FLOAT>(pActiveWindow->GetHeight()), 0.0f, 1.0f };

	if(pRenderTarget != nullptr)
	{
		const RenderTargetRenderDataD3D12* pRenderTargetRenderData = static_cast<RenderTargetRenderDataD3D12*>(pRenderTarget->pRenderData);
		pCmdList->OMSetRenderTargets(pRenderTargetRenderData->numRenderTargetDescriptors, pRenderTargetRenderData->renderTargetDescriptors, TRUE, &pRenderTargetRenderData->depthStencilDescriptor);

		viewport.Width = static_cast<FLOAT>(pRenderTarget->GetWidth());
		viewport.Height = static_cast<FLOAT>(pRenderTarget->GetHeight());
	}
	else
	{
		// TODO: move this to BeginFrame
		{
			// We have to wait for the gpu to finish with the command allocator before we reset it
			WaitForPreviousFrame();

			FrameBuffer& frameBuffer = frameBuffers[activeFrameBufferIdx];
			HRESULT hr = frameBuffer.pCommandAllocator->Reset();
			DX_CHECK_HRESULT(hr);

			// By resetting the command list we are putting it into a recording state so we can start recording commands
			ID3D12PipelineState* pPipelineState = nullptr;
			hr = pCmdList->Reset(frameBuffer.pCommandAllocator, pPipelineState);

			// Transition the render target from present state to render target state
			D3D12_RESOURCE_BARRIER barrier = {};
			barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			barrier.Transition.pResource = frameBuffer.pRenderTargetResource;
			barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
			barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
			barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
			pCmdList->ResourceBarrier(1, &barrier);
		}

		FrameBuffer& frameBuffer = frameBuffers[activeFrameBufferIdx];
		const D3D12_CPU_DESCRIPTOR_HANDLE depthStencilDescriptor = pDepthStencilDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		pCmdList->OMSetRenderTargets(1, &frameBuffer.renderTargetDescriptor, TRUE, &depthStencilDescriptor);
	}

	pCmdList->RSSetViewports(1, &viewport);
}

void Direct3D12Render::UpdateShaderParams(const Material& material)
{
	const ShaderRenderDataD3D12* pVertexShaderRenderData = static_cast<const ShaderRenderDataD3D12*>(pActiveVertexShader->pRenderData);
	UpdateShaderParams(material, pVertexShaderRenderData);

	const ShaderRenderDataD3D12* pFragmentShaderRenderData = static_cast<const ShaderRenderDataD3D12*>(pActiveFragmentShader->pRenderData);
	UpdateShaderParams(material, pFragmentShaderRenderData);
}

void Direct3D12Render::UpdateShaderParams(const Material& material, const ShaderRenderDataD3D12* pShaderRenderData)
{
	const std::pair<uint32_t, uint32_t>* pOffsetSizePair = nullptr;

	for(size_t i = 0; i < pShaderRenderData->constantBuffers.Size(); ++i)
	{
		bool isChanged = false;
		const ShaderRenderDataD3D12::ConstantBufferData& bufferData = pShaderRenderData->constantBuffersData[i];

		for(const Material::ShaderParam& shaderParam : material.GetShaderParams())
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
			float resolution[2] = {};
			if(pActiveRenderTarget != nullptr)
			{
				resolution[0] = pActiveRenderTarget->GetWidth();
				resolution[1] = pActiveRenderTarget->GetHeight();
			}
			else if(pActiveWindow != nullptr)
			{
				resolution[0] = pActiveWindow->GetWidth();
				resolution[1] = pActiveWindow->GetHeight();
			}

			isChanged |= CheckAndUpdateShaderParam(resolution, bufferData.data.ptr.get() + pOffsetSizePair->first, pOffsetSizePair->second);
		}

		if(isChanged)
		{
//			deviceCtx->UpdateSubresource(pShaderRenderData->constantBuffers[i], 0, nullptr, bufferData.data.ptr.get(), 0, 0);
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

void Direct3D12Render::CreateFrameBuffers()
{
	for(uint32_t i = 0; i < kFrameBufferCount; ++i)
	{
		FrameBuffer& frameBuffer = frameBuffers[i];
		HRESULT hr = pSwapChain->GetBuffer(i, IID_PPV_ARGS(&frameBuffer.pRenderTargetResource));
		DX_CHECK_HRESULT(hr);
		pDevice->CreateRenderTargetView(frameBuffer.pRenderTargetResource, nullptr, frameBuffer.renderTargetDescriptor);
	}
}

void Direct3D12Render::WaitForPreviousFrame()
{
	FrameBuffer& frameBuffer = frameBuffers[activeFrameBufferIdx];
	if(frameBuffer.pFence->GetCompletedValue() < frameBuffer.fenceValue)
	{
		// The GPU has not finished executing the command queue so we will wait until the fence has triggered the event
		HRESULT hr = frameBuffer.pFence->SetEventOnCompletion(frameBuffer.fenceValue, fenceEvent);
		DX_CHECK_HRESULT(hr);
		WaitForSingleObject(fenceEvent, INFINITE);
	}

	frameBuffer.fenceValue++;

	activeFrameBufferIdx = pSwapChain->GetCurrentBackBufferIndex();
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
