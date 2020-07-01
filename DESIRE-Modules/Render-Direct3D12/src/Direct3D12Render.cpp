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
#include "Engine/Render/RenderTarget.h"
#include "Engine/Render/View.h"

#include "Engine/Resource/Mesh.h"
#include "Engine/Resource/Shader.h"
#include "Engine/Resource/Texture.h"

#include <dxgi1_4.h>
#include <d3dcompiler.h>

#define DX_RELEASE(ptr)		\
	if(ptr != nullptr)		\
	{						\
		ptr->Release();		\
		ptr = nullptr;		\
	}

Direct3D12Render::Direct3D12Render()
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

void Direct3D12Render::Init(OSWindow* pMainWindow)
{
	HRESULT hr;

#if defined(_DEBUG)
	// Enable the D3D12 debug layer.
	ID3D12Debug* debugController = nullptr;
	hr = D3D12GetDebugInterface(IID_PPV_ARGS(&debugController));
	if(SUCCEEDED(hr))
	{
		debugController->EnableDebugLayer();
		debugController->Release();
	}
#endif

	IDXGIFactory4* factory = nullptr;
	hr = CreateDXGIFactory1(IID_PPV_ARGS(&factory));
	ASSERT(SUCCEEDED(hr));

	hr = D3D12CreateDevice(
		nullptr,
		D3D_FEATURE_LEVEL_12_1,
		IID_PPV_ARGS(&d3dDevice));

	initialized = SUCCEEDED(hr);
	if(!initialized)
	{
		return;
	}

	// Ccreate command queue
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc = {};
	commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	hr = d3dDevice->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue));
	ASSERT(SUCCEEDED(hr));

	// Ccreate swap chain
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.Width = pMainWindow->GetWidth();
	swapChainDesc.Height = pMainWindow->GetHeight();
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.Stereo = FALSE;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 2;
	swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;

	hr = factory->CreateSwapChainForHwnd(commandQueue, static_cast<HWND>(pMainWindow->GetHandle()), &swapChainDesc, nullptr, nullptr, &swapChain);
	ASSERT(SUCCEEDED(hr));

	Bind(screenSpaceQuadVertexShader.get());
	Bind(errorVertexShader.get());
	Bind(errorPixelShader.get());
}

void Direct3D12Render::UpdateRenderWindow(OSWindow* pWindow)
{
	if(!initialized)
	{
		return;
	}

	swapChain->ResizeBuffers(0, pWindow->GetWidth(), pWindow->GetHeight(), DXGI_FORMAT_UNKNOWN, 0);
}

void Direct3D12Render::Kill()
{
	initialized = false;

	pActiveWindow = nullptr;
	pActiveMesh = nullptr;
	pActiveVertexShader = nullptr;
	pActiveFragmentShader = nullptr;

	Unbind(errorVertexShader.get());
	Unbind(errorPixelShader.get());
	Unbind(screenSpaceQuadVertexShader.get());
}

void Direct3D12Render::AppendShaderFilenameWithPath(WritableString& outString, const String& shaderFilename) const
{
	outString += "data/shaders/hlsl/";
	outString += shaderFilename;
	outString += ".hlsl";
}

void Direct3D12Render::BeginFrame(OSWindow* pWindow)
{
	pActiveWindow = pWindow;
	SetViewport(0, 0, pWindow->GetWidth(), pWindow->GetHeight());
}

void Direct3D12Render::EndFrame()
{
	swapChain->Present(1, 0);
}

void Direct3D12Render::SetView(View* pView)
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

//		RenderTargetRenderDataD3D12* pRenderData = static_cast<RenderTargetRenderDataD3D12*>(rt->renderData);
		SetViewport(pView->GetPosX(), pView->GetPosY(), pView->GetWidth(), pView->GetHeight());
	}
	else
	{
		SetViewport(0, 0, pActiveWindow->GetWidth(), pActiveWindow->GetHeight());
	}

	pActiveView = pView;
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

void Direct3D12Render::SetClearColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	clearColor[0] = r / 255.0f;
	clearColor[1] = g / 255.0f;
	clearColor[2] = b / 255.0f;
	clearColor[3] = a / 255.0f;
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

	static const D3D12_BLEND blendConversionTable[] =
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
	DESIRE_CHECK_ARRAY_SIZE(blendConversionTable, EBlend::InvBlendFactor + 1);

	blendDesc.RenderTarget[0].SrcBlend = blendConversionTable[(size_t)srcBlendRGB];
	blendDesc.RenderTarget[0].DestBlend = blendConversionTable[(size_t)destBlendRGB];
	blendDesc.RenderTarget[0].SrcBlendAlpha = blendConversionTable[(size_t)srcBlendAlpha];
	blendDesc.RenderTarget[0].DestBlendAlpha = blendConversionTable[(size_t)destBlendAlpha];

	static const D3D12_BLEND_OP equationConversionTable[] =
	{
		D3D12_BLEND_OP_ADD,				// EBlendOp::Add
		D3D12_BLEND_OP_SUBTRACT,		// EBlendOp::Subtract
		D3D12_BLEND_OP_REV_SUBTRACT,	// EBlendOp::RevSubtract
		D3D12_BLEND_OP_MIN,				// EBlendOp::Min
		D3D12_BLEND_OP_MAX				// EBlendOp::Max
	};
	DESIRE_CHECK_ARRAY_SIZE(equationConversionTable, EBlendOp::Max + 1);

	blendDesc.RenderTarget[0].BlendOp = equationConversionTable[(size_t)blendOpRGB];
	blendDesc.RenderTarget[0].BlendOpAlpha = equationConversionTable[(size_t)blendOpAlpha];
}

void Direct3D12Render::SetBlendModeDisabled()
{
	blendDesc.RenderTarget[0].BlendEnable = FALSE;
}

void Direct3D12Render::Bind(Mesh* pMesh)
{
	ASSERT(pMesh != nullptr);

	if(pMesh->pRenderData != nullptr)
	{
		// Already bound
		return;
	}

	MeshRenderDataD3D12* pRenderData = new MeshRenderDataD3D12();

	pMesh->pRenderData = pRenderData;
}

void Direct3D12Render::Bind(Shader* pShader)
{
	ASSERT(pShader != nullptr);

	if(pShader->renderData != nullptr)
	{
		// Already bound
		return;
	}

	const bool isVertexShader = pShader->name.StartsWith("vs_");

	ShaderRenderDataD3D12* pRenderData = new ShaderRenderDataD3D12();

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
//		hr = d3dDevice->CreateVertexShader(pRenderData->shaderCode->GetBufferPointer(), pRenderData->shaderCode->GetBufferSize(), nullptr, &renderData->vertexShader);
	}
	else
	{
//		hr = d3dDevice->CreatePixelShader(pRenderData->shaderCode->GetBufferPointer(), pRenderData->shaderCode->GetBufferSize(), nullptr, &renderData->pixelShader);
	}
	ASSERT(SUCCEEDED(hr));

//	renderData->ptr->SetPrivateData(WKPDID_D3DDebugObjectName, (UINT)pShader->name.Length(), pShader->name.Str());

	ID3D12ShaderReflection* pReflection = nullptr;
	hr = D3DReflect(pRenderData->shaderCode->GetBufferPointer(), pRenderData->shaderCode->GetBufferSize(), IID_ID3D12ShaderReflection, (void**)&pReflection);
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

	pRenderData->constantBuffers.resize(shaderDesc.ConstantBuffers);
	pRenderData->constantBuffersData.resize(shaderDesc.ConstantBuffers);
/*	for(uint32_t i = 0; i < shaderDesc.ConstantBuffers; ++i)
	{
		ID3D12ShaderReflectionConstantBuffer* cbuffer = pReflection->GetConstantBufferByIndex(i);
		D3D12_SHADER_BUFFER_DESC shaderBufferDesc;
		hr = cbuffer->GetDesc(&shaderBufferDesc);
		ASSERT(SUCCEEDED(hr));

		// Create constant buffer
		D3D12_BUFFER_DESC bufferDesc = {};
		bufferDesc.ByteWidth = shaderBufferDesc.Size;
		bufferDesc.Usage = D3D12_USAGE_DEFAULT;
		bufferDesc.BindFlags = D3D12_BIND_CONSTANT_BUFFER;
		hr = d3dDevice->CreateCommittedResource(&bufferDesc, nullptr, &renderData->constantBuffers[i]);
		ASSERT(SUCCEEDED(hr));

		// Create constant buffer data
		ShaderRenderDataD3D12::ConstantBufferData& bufferData = pRenderData->constantBuffersData[i];
		bufferData.buffer = MemoryBuffer(shaderBufferDesc.Size);

		for(uint32_t j = 0; j < shaderBufferDesc.Variables; ++j)
		{
			ID3D12ShaderReflectionVariable* shaderVar = cbuffer->GetVariableByIndex(j);
			D3D12_SHADER_VARIABLE_DESC varDesc;
			hr = shaderVar->GetDesc(&varDesc);
			ASSERT(SUCCEEDED(hr));

			if((varDesc.uFlags & D3D_SVF_USED) == 0)
			{
				continue;
			}

			ID3D12ShaderReflectionType* type = shaderVar->GetType();
			D3D12_SHADER_TYPE_DESC typeDesc;
			hr = type->GetDesc(&typeDesc);
			ASSERT(SUCCEEDED(hr));

			if( typeDesc.Class != D3D_SVC_SCALAR &&
				typeDesc.Class != D3D_SVC_VECTOR &&
				typeDesc.Class != D3D_SVC_MATRIX_COLUMNS)
			{
				continue;
			}

			bufferData.variableOffsetSizePairs.Insert(HashedString::CreateFromDynamicString(varDesc.Name), std::make_pair(varDesc.StartOffset, varDesc.Size));
		}
	}
*/

	DX_RELEASE(pReflection);

	pShader->renderData = pRenderData;
}

void Direct3D12Render::Bind(Texture* texture)
{
	ASSERT(texture != nullptr);

	if(texture->renderData != nullptr)
	{
		// Already bound
		return;
	}

	TextureRenderDataD3D12* pRenderData = new TextureRenderDataD3D12();

	texture->renderData = pRenderData;
}

void Direct3D12Render::Bind(RenderTarget* renderTarget)
{
	ASSERT(renderTarget != nullptr);

	if(renderTarget->renderData != nullptr)
	{
		// Already bound
		return;
	}

	RenderTargetRenderDataD3D12* pRenderData = new RenderTargetRenderDataD3D12();

	renderTarget->renderData = pRenderData;
}

void Direct3D12Render::Unbind(Mesh* pMesh)
{
	if(pMesh == nullptr || pMesh->pRenderData == nullptr)
	{
		// Not yet bound
		return;
	}

	MeshRenderDataD3D12* pRenderData = static_cast<MeshRenderDataD3D12*>(pMesh->pRenderData);

	delete pRenderData;
	pMesh->pRenderData = nullptr;

	if(pActiveMesh == pMesh)
	{
		pActiveMesh = nullptr;
	}
}

void Direct3D12Render::Unbind(Shader* pShader)
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

	ShaderRenderDataD3D12* pRenderData = static_cast<ShaderRenderDataD3D12*>(pShader->renderData);

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

void Direct3D12Render::Unbind(Texture* texture)
{
	if(texture == nullptr || texture->renderData == nullptr)
	{
		// Not yet bound
		return;
	}

	TextureRenderDataD3D12* pRenderData = static_cast<TextureRenderDataD3D12*>(texture->renderData);

	delete pRenderData;
	texture->renderData = nullptr;
}

void Direct3D12Render::Unbind(RenderTarget* renderTarget)
{
	if(renderTarget == nullptr || renderTarget->renderData == nullptr)
	{
		// Not yet bound
		return;
	}

	RenderTargetRenderDataD3D12* pRenderData = static_cast<RenderTargetRenderDataD3D12*>(renderTarget->renderData);

	const uint8_t textureCount = std::min<uint8_t>(renderTarget->GetTextureCount(), D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT);
	for(uint8_t i = 0; i < textureCount; ++i)
	{
		Unbind(renderTarget->GetTexture(i).get());
	}

	delete pRenderData;
	renderTarget->renderData = nullptr;
}

void Direct3D12Render::SetViewport(uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
	const D3D12_VIEWPORT vp = { (float)x, (float)y, (float)width, (float)height, 0.0f, 1.0f };
}

void Direct3D12Render::SetMesh(Mesh* pMesh)
{
	if(pMesh != nullptr)
	{
//		MeshRenderDataD3D12* pRenderData = static_cast<MeshRenderDataD3D12*>(pMesh->pRenderData);
	}
	else
	{
	}
}

void Direct3D12Render::UpdateDynamicMesh(DynamicMesh& dynamicMesh)
{
	if(dynamicMesh.isIndicesDirty)
	{
	}

	if(dynamicMesh.isVerticesDirty)
	{
	}
}

void Direct3D12Render::SetVertexShader(Shader* pVertexShader)
{
	if(pActiveVertexShader == pVertexShader)
	{
		return;
	}

//	const ShaderRenderDataD3D12* vertexShaderRenderData = static_cast<const ShaderRenderDataD3D12*>(vertexShader->renderData);

	pActiveVertexShader = pVertexShader;
}

void Direct3D12Render::SetFragmentShader(Shader* pFragmentShader)
{
	if(pActiveFragmentShader == pFragmentShader)
	{
		return;
	}

//	const ShaderRenderDataD3D12* shaderRenderData = static_cast<const ShaderRenderDataD3D12*>(fragmentShader->renderData);

	pActiveFragmentShader = pFragmentShader;
}

void Direct3D12Render::SetTexture(uint8_t samplerIdx, Texture* texture, EFilterMode filterMode, EAddressMode addressMode)
{
	DESIRE_UNUSED(samplerIdx);

	ASSERT(samplerIdx < D3D12_COMMONSHADER_SAMPLER_SLOT_COUNT);

//	TextureRenderDataD3D12* pRenderData = static_cast<TextureRenderDataD3D12*>(texture->renderData);

	static const D3D12_TEXTURE_ADDRESS_MODE addressModeConversionTable[] =
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
	};
	samplerDesc.AddressU = addressModeConversionTable[(size_t)addressMode];
	samplerDesc.AddressV = addressModeConversionTable[(size_t)addressMode];
	samplerDesc.AddressW = addressModeConversionTable[(size_t)addressMode];
	samplerDesc.MaxAnisotropy = D3D12_MAX_MAXANISOTROPY;
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;

	DESIRE_UNUSED(texture);
}

void Direct3D12Render::UpdateShaderParams(const Material* pMaterial)
{
	const ShaderRenderDataD3D12* pVertexShaderRenderData = static_cast<const ShaderRenderDataD3D12*>(pActiveVertexShader->renderData);
	UpdateShaderParams(pMaterial, pVertexShaderRenderData);

	const ShaderRenderDataD3D12* pFragmentShaderRenderData = static_cast<const ShaderRenderDataD3D12*>(pActiveFragmentShader->renderData);
	UpdateShaderParams(pMaterial, pFragmentShaderRenderData);
}

void Direct3D12Render::UpdateShaderParams(const Material* pMaterial, const ShaderRenderDataD3D12* pShaderRenderData)
{
	const std::pair<uint32_t, uint32_t>* pOffsetSizePair = nullptr;

	for(size_t i = 0; i < pShaderRenderData->constantBuffers.size(); ++i)
	{
		bool isChanged = false;
		const ShaderRenderDataD3D12::ConstantBufferData& bufferData = pShaderRenderData->constantBuffersData[i];

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

void Direct3D12Render::DoRender(uint32_t indexOffset, uint32_t vertexOffset, uint32_t numIndices, uint32_t numVertices)
{
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

DXGI_FORMAT Direct3D12Render::GetTextureFormat(const Texture* pTexture)
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

	return conversionTable[static_cast<size_t>(pTexture->format)];
}
