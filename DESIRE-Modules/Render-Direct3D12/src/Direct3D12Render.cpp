#include "stdafx_Direct3D12.h"
#include "Direct3D12Render.h"

#include "DirectXMathExt.h"
#include "MeshRenderDataD3D12.h"
#include "RenderableRenderDataD3D12.h"
#include "RenderTargetRenderDataD3D12.h"
#include "ShaderRenderDataD3D12.h"
#include "TextureRenderDataD3D12.h"
#include "ToD3D12.h"

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

	m_activeFrameBufferIdx = m_pSwapChain->GetCurrentBackBufferIndex();

	hr = pFactory->MakeWindowAssociation(static_cast<HWND>(mainWindow.GetHandle()), DXGI_MWA_NO_ALT_ENTER);
	DX_CHECK_HRESULT(hr);

	bool succeeded = CreateFrameBuffers(mainWindow.GetWidth(), mainWindow.GetHeight());
	if(!succeeded)
	{
		return false;
	}

	m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if(m_fenceEvent == nullptr)
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

	// Create root signature
	CD3DX12_DESCRIPTOR_RANGE1 t0;
	t0.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);		// t0

	// Perfomance TIP: Order from most frequent to least frequent
	CD3DX12_ROOT_PARAMETER1 rootParameters[2] = {};
	rootParameters[0].InitAsConstantBufferView(0);
	rootParameters[1].InitAsDescriptorTable(1, &t0, D3D12_SHADER_VISIBILITY_PIXEL);

	CD3DX12_STATIC_SAMPLER_DESC staticSamplers[1];
	staticSamplers->Init(0);

	D3D12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
	rootSignatureDesc.Version = D3D_ROOT_SIGNATURE_VERSION_1_1;
	rootSignatureDesc.Desc_1_1.NumParameters = static_cast<UINT>(DESIRE_ASIZEOF(rootParameters));
	rootSignatureDesc.Desc_1_1.pParameters = rootParameters;
	rootSignatureDesc.Desc_1_1.NumStaticSamplers = static_cast<UINT>(DESIRE_ASIZEOF(staticSamplers));
	rootSignatureDesc.Desc_1_1.pStaticSamplers = staticSamplers;
	rootSignatureDesc.Desc_1_1.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	ID3DBlob* pSignature = nullptr;
	ID3DBlob* pErrorBlob = nullptr;
	hr = D3D12SerializeVersionedRootSignature(&rootSignatureDesc, &pSignature, &pErrorBlob);
	if(FAILED(hr))
	{
		if(pErrorBlob != nullptr)
		{
			LOG_ERROR("Root signature serialize error:\n%s", static_cast<const char*>(pErrorBlob->GetBufferPointer()));
			DX_RELEASE(pErrorBlob);
		}

		return false;
	}

	hr = m_pDevice->CreateRootSignature(0, pSignature->GetBufferPointer(), pSignature->GetBufferSize(), IID_PPV_ARGS(&m_pRootSignature));
	DX_CHECK_HRESULT(hr);

	m_pCmdList->SetGraphicsRootSignature(m_pRootSignature);

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
	m_pActiveRenderTarget = nullptr;

	m_pActivePipelineState = nullptr;

	DX_RELEASE(m_pRootSignature);

	DX_RELEASE(m_pCmdList);

	CloseHandle(m_fenceEvent);

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
			for(uint32_t i = 0; i < pRenderTargetRenderData->m_numRTVs; ++i)
			{
				m_pCmdList->ClearRenderTargetView(pRenderTargetRenderData->m_RTVs[i], clearColor, 0, nullptr);
			}

			m_pCmdList->ClearDepthStencilView(pRenderTargetRenderData->m_DSV, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, depth, stencil, 0, nullptr);
		}
	}
	else
	{
		FrameBuffer& frameBuffer = m_frameBuffers[m_activeFrameBufferIdx];
		m_pCmdList->ClearRenderTargetView(frameBuffer.m_RTV, clearColor, 0, nullptr);
		m_pCmdList->ClearDepthStencilView(m_pHeapForDSV->GetCPUDescriptorHandleForHeapStart(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, depth, stencil, 0, nullptr);
	}
}

void Direct3D12Render::SetScissor(uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
	const D3D12_RECT rect = { x, y, x + width, y + height };
	m_pCmdList->RSSetScissorRects(1, &rect);
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

void* Direct3D12Render::CreateRenderableRenderData(const Renderable& renderable)
{
	RenderableRenderDataD3D12* pRenderableRenderData = new RenderableRenderDataD3D12();

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.pRootSignature = m_pRootSignature;
	psoDesc.VS = CD3DX12_SHADER_BYTECODE(static_cast<ShaderRenderDataD3D12*>(renderable.m_material->m_vertexShader->m_pRenderData)->m_pShaderCode);
	psoDesc.PS = CD3DX12_SHADER_BYTECODE(static_cast<ShaderRenderDataD3D12*>(renderable.m_material->m_pixelShader->m_pRenderData)->m_pShaderCode);
//	D3D12_STREAM_OUTPUT_DESC StreamOutput;
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.SampleMask = UINT32_MAX;
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	psoDesc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	psoDesc.SampleDesc.Count = 1;
//	D3D12_CACHED_PIPELINE_STATE CachedPSO;
	psoDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	// Input Layout
	const Array<Mesh::VertexLayout>& vertexLayout = renderable.m_mesh->GetVertexLayout();
	D3D12_INPUT_ELEMENT_DESC vertexElementDesc[static_cast<size_t>(Mesh::EAttrib::Num)] = {};
	for(size_t i = 0; i < vertexLayout.Size(); ++i)
	{
		const Mesh::VertexLayout& layout = vertexLayout[i];
		vertexElementDesc[i] = ToD3D12(layout.m_attrib);
		vertexElementDesc[i].Format = ToD3D12(layout.m_type, layout.m_count);
	}

	psoDesc.InputLayout.pInputElementDescs = vertexElementDesc;
	psoDesc.InputLayout.NumElements = static_cast<UINT>(vertexLayout.Size());

	// Rasterizer State
	psoDesc.RasterizerState.DepthClipEnable = FALSE;
	psoDesc.RasterizerState.MultisampleEnable = TRUE;
	psoDesc.RasterizerState.AntialiasedLineEnable = TRUE;

	switch(renderable.m_material->m_cullMode)
	{
		case ECullMode::None:	psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE; break;
		case ECullMode::CCW:	psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK; break;
		case ECullMode::CW:		psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_FRONT; break;
	}

	// Blend State
	if(renderable.m_material->m_isBlendEnabled)
	{
		psoDesc.BlendState.RenderTarget[0].BlendEnable = TRUE;
		psoDesc.BlendState.RenderTarget[0].SrcBlend = ToD3D12(renderable.m_material->m_srcBlendRGB);
		psoDesc.BlendState.RenderTarget[0].DestBlend = ToD3D12(renderable.m_material->m_destBlendRGB);
		psoDesc.BlendState.RenderTarget[0].BlendOp = ToD3D12(renderable.m_material->m_blendOpRGB);
		psoDesc.BlendState.RenderTarget[0].SrcBlendAlpha = ToD3D12(renderable.m_material->m_srcBlendAlpha);
		psoDesc.BlendState.RenderTarget[0].DestBlendAlpha = ToD3D12(renderable.m_material->m_destBlendAlpha);
		psoDesc.BlendState.RenderTarget[0].BlendOpAlpha = ToD3D12(renderable.m_material->m_blendOpAlpha);
	}

	static_assert(static_cast<uint8_t>(EColorWrite::Red) == D3D12_COLOR_WRITE_ENABLE_RED);
	static_assert(static_cast<uint8_t>(EColorWrite::Green) == D3D12_COLOR_WRITE_ENABLE_GREEN);
	static_assert(static_cast<uint8_t>(EColorWrite::Blue) == D3D12_COLOR_WRITE_ENABLE_BLUE);
	static_assert(static_cast<uint8_t>(EColorWrite::Alpha) == D3D12_COLOR_WRITE_ENABLE_ALPHA);

	psoDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = static_cast<uint8_t>(renderable.m_material->m_colorWriteMask);

	// Depth Stencil State
	switch(renderable.m_material->m_depthTest)
	{
		case EDepthTest::Disabled:		psoDesc.DepthStencilState.DepthEnable = FALSE; break;
		case EDepthTest::Less:			psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS; break;
		case EDepthTest::LessEqual:		psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL; break;
		case EDepthTest::Greater:		psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_GREATER; break;
		case EDepthTest::GreaterEqual:	psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_GREATER_EQUAL; break;
		case EDepthTest::Equal:			psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_EQUAL; break;
		case EDepthTest::NotEqual:		psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_NOT_EQUAL; break;
	}

	psoDesc.DepthStencilState.DepthWriteMask = renderable.m_material->m_isDepthWriteEnabled ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO;

	// Pipeline State
	HRESULT hr = m_pDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pRenderableRenderData->m_pPipelineState));
	DX_CHECK_HRESULT(hr);

	return pRenderableRenderData;
}

void* Direct3D12Render::CreateMeshRenderData(const Mesh& mesh)
{
	MeshRenderDataD3D12* pMeshRenderData = new MeshRenderDataD3D12();

	const CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_DEFAULT);

	// Create index buffer
	if(mesh.GetNumIndices() != 0)
	{
		CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(mesh.GetSizeOfIndexData());

		HRESULT hr = m_pDevice->CreateCommittedResource(
			&heapProperties,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(&pMeshRenderData->m_pIndexBuffer)
		);
		DX_CHECK_HRESULT(hr);

		pMeshRenderData->m_indexBufferView.BufferLocation = pMeshRenderData->m_pIndexBuffer->GetGPUVirtualAddress();
		pMeshRenderData->m_indexBufferView.SizeInBytes = mesh.GetSizeOfIndexData();
		pMeshRenderData->m_indexBufferView.Format = DXGI_FORMAT_R16_UINT;

		// Create index upload buffer
		CD3DX12_HEAP_PROPERTIES uploadHeapProperties(D3D12_HEAP_TYPE_UPLOAD);
		CD3DX12_RESOURCE_DESC uploadResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(GetRequiredIntermediateSize(pMeshRenderData->m_pIndexBuffer, 0, 1));

		ID3D12Resource* pUploadResource = nullptr;
		hr = m_pDevice->CreateCommittedResource(
			&uploadHeapProperties,
			D3D12_HEAP_FLAG_NONE,
			&uploadResourceDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&pUploadResource)
		);
		DX_CHECK_HRESULT(hr);

		D3D12_SUBRESOURCE_DATA indexData = {};
		indexData.pData = mesh.m_indices.get();

		UpdateSubresources<1>(m_pCmdList, pMeshRenderData->m_pIndexBuffer, pUploadResource, 0, 0, 1, &indexData);
	}

	// Create vertex buffer
	CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(mesh.GetSizeOfVertexData());

	HRESULT hr = m_pDevice->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(&pMeshRenderData->m_pVertexBuffer)
	);
	DX_CHECK_HRESULT(hr);

	pMeshRenderData->m_vertexBufferView.BufferLocation = pMeshRenderData->m_pVertexBuffer->GetGPUVirtualAddress();
	pMeshRenderData->m_vertexBufferView.SizeInBytes = mesh.GetSizeOfVertexData();
	pMeshRenderData->m_vertexBufferView.StrideInBytes = mesh.GetVertexSize();

	// Create vertex upload buffer
	CD3DX12_HEAP_PROPERTIES uploadHeapProperties(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC uploadResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(GetRequiredIntermediateSize(pMeshRenderData->m_pVertexBuffer, 0, 1));

	ID3D12Resource* pUploadResource = nullptr;
	hr = m_pDevice->CreateCommittedResource(
		&uploadHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&uploadResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&pUploadResource)
	);
	DX_CHECK_HRESULT(hr);

	D3D12_SUBRESOURCE_DATA vertexData = {};
	vertexData.pData = mesh.m_vertices.get();

	UpdateSubresources<1>(m_pCmdList, pMeshRenderData->m_pVertexBuffer, pUploadResource, 0, 0, 1, &vertexData);

	const CD3DX12_RESOURCE_BARRIER barriers[] =
	{
		CD3DX12_RESOURCE_BARRIER::Transition(pMeshRenderData->m_pVertexBuffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER),
		CD3DX12_RESOURCE_BARRIER::Transition(pMeshRenderData->m_pIndexBuffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_INDEX_BUFFER)
	};

	const uint32_t numBarriers = (mesh.GetNumIndices() != 0) ? 2 : 1;
	m_pCmdList->ResourceBarrier(numBarriers, barriers);

	// TODO: this is a memory leak should be solved by executing the command list here
//	DX_RELEASE(pUploadResource);

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

	ID3D12ShaderReflection* pReflection = nullptr;
	hr = D3DReflect(pShaderRenderData->m_pShaderCode->GetBufferPointer(), pShaderRenderData->m_pShaderCode->GetBufferSize(), IID_PPV_ARGS(&pReflection));
	if(FAILED(hr))
	{
		LOG_ERROR("D3DReflect failed 0x%08x\n", static_cast<uint32_t>(hr));
	}

	D3D12_SHADER_DESC shaderDesc;
	hr = pReflection->GetDesc(&shaderDesc);
	if(FAILED(hr))
	{
		LOG_ERROR("ID3D12ShaderReflection::GetDesc failed 0x%08x\n", static_cast<uint32_t>(hr));
	}

	pShaderRenderData->m_constantBuffers.SetSize(shaderDesc.ConstantBuffers);
	pShaderRenderData->m_constantBuffersData.SetSize(shaderDesc.ConstantBuffers);
	for(uint32_t i = 0; i < shaderDesc.ConstantBuffers; ++i)
	{
		ID3D12ShaderReflectionConstantBuffer* pConstantBuffer = pReflection->GetConstantBufferByIndex(i);
		D3D12_SHADER_BUFFER_DESC shaderBufferDesc;
		hr = pConstantBuffer->GetDesc(&shaderBufferDesc);
		DX_CHECK_HRESULT(hr);

		if(shaderBufferDesc.Type == D3D_CT_CBUFFER || shaderBufferDesc.Type == D3D_CT_TBUFFER)
		{
			// Create constant buffer
			CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_DEFAULT);
			CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(shaderBufferDesc.Size);

			hr = m_pDevice->CreateCommittedResource(
				&heapProperties,
				D3D12_HEAP_FLAG_NONE,
				&resourceDesc,
				D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
				nullptr,
				IID_PPV_ARGS(&pShaderRenderData->m_constantBuffers[i])
			);
			DX_CHECK_HRESULT(hr);

			// Create constant buffer data
			ShaderRenderDataD3D12::ConstantBufferData& bufferData = pShaderRenderData->m_constantBuffersData[i];
			bufferData.m_data = MemoryBuffer(shaderBufferDesc.Size);

			for(uint32_t j = 0; j < shaderBufferDesc.Variables; ++j)
			{
				ID3D12ShaderReflectionVariable* pVariable = pConstantBuffer->GetVariableByIndex(j);
				D3D12_SHADER_VARIABLE_DESC varDesc;
				hr = pVariable->GetDesc(&varDesc);
				DX_CHECK_HRESULT(hr);

				if((varDesc.uFlags & D3D_SVF_USED) == 0)
				{
					continue;
				}

				ID3D12ShaderReflectionType* pType = pVariable->GetType();
				D3D12_SHADER_TYPE_DESC typeDesc;
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

void* Direct3D12Render::CreateTextureRenderData(const Texture& texture)
{
	TextureRenderDataD3D12* pTextureRenderData = new TextureRenderDataD3D12();

	CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_DEFAULT);
	CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(ToD3D12(texture.GetFormat()), texture.GetWidth(), texture.GetHeight(), 1, texture.GetNumMipLevels());

	HRESULT hr = m_pDevice->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(&pTextureRenderData->m_pTexture)
	);
	DX_CHECK_HRESULT(hr);

	// Create the upload buffer
	CD3DX12_HEAP_PROPERTIES uploadHeapProperties(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC uploadResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(GetRequiredIntermediateSize(pTextureRenderData->m_pTexture, 0, 1));

	ID3D12Resource* pUploadResource = nullptr;
	hr = m_pDevice->CreateCommittedResource(
		&uploadHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&uploadResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&pUploadResource)
	);
	DX_CHECK_HRESULT(hr);

	const uint32_t numTextureData = resourceDesc.MipLevels * resourceDesc.ArraySize();
	DESIRE_STACKALLOCATE_ARRAY(D3D12_SUBRESOURCE_DATA, textureData, numTextureData);
	ASSERT(numTextureData == 1 && "TODO: Set pData properly in the loop below");
	for(uint32_t i = 0; i < numTextureData; ++i)
	{
		textureData[i].pData = texture.GetData();
		textureData[i].RowPitch = texture.GetWidth() * texture.GetBytesPerPixel();
		textureData[i].SlicePitch = textureData[i].RowPitch * texture.GetHeight();
	}

	UpdateSubresources(m_pCmdList, pTextureRenderData->m_pTexture, pUploadResource, 0, 0, numTextureData, textureData);

	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(pTextureRenderData->m_pTexture, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	m_pCmdList->ResourceBarrier(1, &barrier);

	// Create SRV
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.NumDescriptors = 1;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	hr = m_pDevice->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&pTextureRenderData->m_pHeapForSRV));
	DX_CHECK_HRESULT(hr);

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = resourceDesc.Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Texture2D.MipLevels = resourceDesc.MipLevels;

	m_pDevice->CreateShaderResourceView(pTextureRenderData->m_pTexture, &srvDesc, pTextureRenderData->m_pHeapForSRV->GetCPUDescriptorHandleForHeapStart());

	// TODO: this is a memory leak should be solved by executing the command list here
//	DX_RELEASE(pUploadResource);

	return pTextureRenderData;
}

void* Direct3D12Render::CreateRenderTargetRenderData(const RenderTarget& renderTarget)
{
	RenderTargetRenderDataD3D12* pRenderTargetRenderData = new RenderTargetRenderDataD3D12();

	const uint8_t textureCount = std::min<uint8_t>(renderTarget.GetTextureCount(), D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT);

	bool hasDepthTexture = false;
	for(uint8_t i = 0; i < textureCount; ++i)
	{
		if(renderTarget.GetTexture(i)->IsDepthFormat())
		{
			hasDepthTexture = true;
			break;
		}
	}

	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.NumDescriptors = hasDepthTexture ? textureCount - 1 : textureCount;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	HRESULT hr = m_pDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&pRenderTargetRenderData->m_pHeapForRTVs));
	DX_CHECK_HRESULT(hr);

	const UINT descriptorHandleIncrementSize = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	D3D12_CPU_DESCRIPTOR_HANDLE currRTV = m_pHeapForFrameBufferRTVs->GetCPUDescriptorHandleForHeapStart();
	for(uint8_t i = 0; i < textureCount; ++i)
	{
		const std::shared_ptr<Texture>& texture = renderTarget.GetTexture(i);
		const TextureRenderDataD3D12* pTextureRenderData = static_cast<const TextureRenderDataD3D12*>(texture->m_pRenderData);
		ASSERT(pTextureRenderData != nullptr);

		if(texture->IsDepthFormat())
		{
			ASSERT(pRenderTargetRenderData->m_pHeapForDSV == nullptr && "RenderTargets can have only 1 depth attachment");

			D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
			dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
			dsvHeapDesc.NumDescriptors = 1;
			dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

			hr = m_pDevice->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&pRenderTargetRenderData->m_pHeapForDSV));
			DX_CHECK_HRESULT(hr);

			m_pDevice->CreateDepthStencilView(pTextureRenderData->m_pTexture, nullptr, pRenderTargetRenderData->m_pHeapForDSV->GetCPUDescriptorHandleForHeapStart());
		}
		else
		{
			m_pDevice->CreateRenderTargetView(pTextureRenderData->m_pTexture, nullptr, currRTV);

			pRenderTargetRenderData->m_RTVs[pRenderTargetRenderData->m_numRTVs++] = currRTV;
			currRTV.ptr += descriptorHandleIncrementSize;
		}
	}

	return pRenderTargetRenderData;
}

void Direct3D12Render::DestroyRenderableRenderData(void* pRenderData)
{
	RenderableRenderDataD3D12* pRenderableRenderData = static_cast<RenderableRenderDataD3D12*>(pRenderData);

	if(m_pActivePipelineState == pRenderableRenderData->m_pPipelineState)
	{
		m_pActivePipelineState = nullptr;
	}

	DX_RELEASE(pRenderableRenderData->m_pPipelineState);

	delete pRenderableRenderData;
}

void Direct3D12Render::DestroyMeshRenderData(void* pRenderData)
{
	MeshRenderDataD3D12* pMeshRenderData = static_cast<MeshRenderDataD3D12*>(pRenderData);

	DX_RELEASE(pMeshRenderData->m_pIndexBuffer);
	DX_RELEASE(pMeshRenderData->m_pVertexBuffer);

	delete pMeshRenderData;
}

void Direct3D12Render::DestroyShaderRenderData(void* pRenderData)
{
	ShaderRenderDataD3D12* pShaderRenderData = static_cast<ShaderRenderDataD3D12*>(pRenderData);

	if(pShaderRenderData == m_errorVertexShader->m_pRenderData || pShaderRenderData == m_errorPixelShader->m_pRenderData)
	{
		return;
	}

	DX_RELEASE(pShaderRenderData->m_pShaderCode);

	delete pShaderRenderData;
}

void Direct3D12Render::DestroyTextureRenderData(void* pRenderData)
{
	TextureRenderDataD3D12* pTextureRenderData = static_cast<TextureRenderDataD3D12*>(pRenderData);

	DX_RELEASE(pTextureRenderData->m_pTexture);
	DX_RELEASE(pTextureRenderData->m_pHeapForSRV);

	delete pTextureRenderData;
}

void Direct3D12Render::DestroyRenderTargetRenderData(void* pRenderData)
{
	RenderTargetRenderDataD3D12* pRenderTargetRenderData = static_cast<RenderTargetRenderDataD3D12*>(pRenderData);

	pRenderTargetRenderData->m_pHeapForRTVs->Release();
	pRenderTargetRenderData->m_pHeapForDSV->Release();

	delete pRenderTargetRenderData;
}

void Direct3D12Render::SetMesh(Mesh& mesh)
{
	const MeshRenderDataD3D12* pMeshRenderData = static_cast<const MeshRenderDataD3D12*>(mesh.m_pRenderData);
	m_pCmdList->IASetIndexBuffer(&pMeshRenderData->m_indexBufferView);
	m_pCmdList->IASetVertexBuffers(0, 1, &pMeshRenderData->m_vertexBufferView);
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
		m_pCmdList->OMSetRenderTargets(pRenderTargetRenderData->m_numRTVs, pRenderTargetRenderData->m_RTVs, TRUE, &pRenderTargetRenderData->m_DSV);

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
			hr = m_pCmdList->Reset(frameBuffer.m_pCommandAllocator, m_pActivePipelineState);

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
	ShaderRenderDataD3D12* pVS = static_cast<ShaderRenderDataD3D12*>(material.m_vertexShader->m_pRenderData);
	UpdateShaderParams(material, pVS);

	ShaderRenderDataD3D12* pPS = static_cast<ShaderRenderDataD3D12*>(material.m_pixelShader->m_pRenderData);
	UpdateShaderParams(material, pPS);
}

void Direct3D12Render::UpdateShaderParams(const Material& material, ShaderRenderDataD3D12* pShaderRenderData)
{
	ShaderRenderDataD3D12::ConstantBufferData::Variable* pVariable = nullptr;

	for(size_t i = 0; i < pShaderRenderData->m_constantBuffers.Size(); ++i)
	{
		bool isChanged = false;
		ShaderRenderDataD3D12::ConstantBufferData& bufferData = pShaderRenderData->m_constantBuffersData[i];

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
//			deviceCtx->UpdateSubresource(pShaderRenderData->m_constantBuffers[i], 0, nullptr, bufferData.m_data.ptr.get(), 0, 0);
		}
	}
}

void Direct3D12Render::DoRender(Renderable& renderable, uint32_t indexOffset, uint32_t vertexOffset, uint32_t numIndices, uint32_t numVertices)
{
	RenderableRenderDataD3D12* pRenderableRenderData = static_cast<RenderableRenderDataD3D12*>(renderable.m_pRenderData);

	if(m_pActivePipelineState != pRenderableRenderData->m_pPipelineState)
	{
		m_pCmdList->SetPipelineState(pRenderableRenderData->m_pPipelineState);
		m_pActivePipelineState = pRenderableRenderData->m_pPipelineState;
	}

	if(numIndices != 0)
	{
		m_pCmdList->DrawIndexedInstanced(numIndices, 1, indexOffset, vertexOffset, 0);
	}
	else
	{
		m_pCmdList->DrawInstanced(numVertices, 1, vertexOffset, 0);
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
	CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D24_UNORM_S8_UINT, width, height);
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
	depthOptimizedClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
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

	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = resourceDesc.Format;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

	m_pDevice->CreateDepthStencilView(m_pDepthStencilResource, &dsvDesc, m_pHeapForDSV->GetCPUDescriptorHandleForHeapStart());

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
