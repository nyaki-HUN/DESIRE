#include "Direct3D12Render.h"
#include "DirectXMathExt.h"
#include "MeshRenderDataD3D12.h"
#include "ShaderRenderDataD3D12.h"
#include "TextureRenderDataD3D12.h"
#include "RenderTargetRenderDataD3D12.h"

#include "Engine/Application/OSWindow.h"
#include "Engine/Core/assert.h"
#include "Engine/Core/Log.h"
#include "Engine/Core/fs/FileSystem.h"
#include "Engine/Core/math/Matrix4.h"
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

void Direct3D12Render::Init(OSWindow *mainWindow)
{
	HRESULT hr;

#if defined(_DEBUG)
	// Enable the D3D12 debug layer.
	ID3D12Debug *debugController = nullptr;
	hr = D3D12GetDebugInterface(IID_PPV_ARGS(&debugController));
	if(SUCCEEDED(hr))
	{
		debugController->EnableDebugLayer();
		debugController->Release();
	}
#endif

	IDXGIFactory4 *factory = nullptr;
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
	swapChainDesc.Width = mainWindow->GetWidth();
	swapChainDesc.Height = mainWindow->GetHeight();
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.Stereo = FALSE;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 2;
	swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;

	hr = factory->CreateSwapChainForHwnd(commandQueue, (HWND)mainWindow->GetHandle(), &swapChainDesc, nullptr, nullptr, &swapChain);
	ASSERT(SUCCEEDED(hr));

	Bind(screenSpaceQuadVertexShader.get());
	Bind(errorVertexShader.get());
	Bind(errorPixelShader.get());
}

void Direct3D12Render::UpdateRenderWindow(OSWindow *window)
{
	if(!initialized)
	{
		return;
	}

	swapChain->ResizeBuffers(0, window->GetWidth(), window->GetHeight(), DXGI_FORMAT_UNKNOWN, 0);
}

void Direct3D12Render::Kill()
{
	initialized = false;

	activeWindow = nullptr;
	activeMesh = nullptr;
	activeVertexShader = nullptr;
	activeFragmentShader = nullptr;

	Unbind(errorVertexShader.get());
	Unbind(errorPixelShader.get());
	Unbind(screenSpaceQuadVertexShader.get());
}

DynamicString Direct3D12Render::GetShaderFilenameWithPath(const String& shaderFilename) const
{
	DynamicString filenameWithPath = "data/shaders/hlsl/";
	filenameWithPath += shaderFilename;
	filenameWithPath += ".hlsl";
	return filenameWithPath;
}

void Direct3D12Render::BeginFrame(OSWindow *window)
{
	activeWindow = window;
	SetViewport(0, 0, window->GetWidth(), window->GetHeight());
}

void Direct3D12Render::EndFrame()
{
	swapChain->Present(1, 0);
}

void Direct3D12Render::SetView(View *view)
{
	if(view == activeView)
	{
		return;
	}

	if(view != nullptr)
	{
		RenderTarget *rt = view->GetRenderTarget();
		if(rt->renderData == nullptr)
		{
			Bind(rt);
		}

		RenderTargetRenderDataD3D12 *renderData = static_cast<RenderTargetRenderDataD3D12*>(rt->renderData);
		SetViewport(view->GetPosX(), view->GetPosY(), view->GetWidth(), view->GetHeight());
	}
	else
	{
		SetViewport(0, 0, activeWindow->GetWidth(), activeWindow->GetHeight());
	}

	activeView = view;
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

void Direct3D12Render::Bind(Mesh *mesh)
{
	ASSERT(mesh != nullptr);

	if(mesh->renderData != nullptr)
	{
		// Already bound
		return;
	}

	MeshRenderDataD3D12 *renderData = new MeshRenderDataD3D12();

	mesh->renderData = renderData;
}

void Direct3D12Render::Bind(Shader *shader)
{
	ASSERT(shader != nullptr);

	if(shader->renderData != nullptr)
	{
		// Already bound
		return;
	}

	const bool isVertexShader = shader->name.StartsWith("vs_");
	
	ShaderRenderDataD3D12 *renderData = new ShaderRenderDataD3D12();

	D3D_SHADER_MACRO defines[32] = {};
	ASSERT(shader->defines.size() < DESIRE_ASIZEOF(defines));
	D3D_SHADER_MACRO *definePtr = &defines[0];
	for(const String& define : shader->defines)
	{
		definePtr->Name = define.Str();
		definePtr->Definition = "1";
		definePtr++;
	}

	ID3DBlob *errorBlob = nullptr;
	HRESULT hr = D3DCompile(shader->data.data														// pSrcData
		, shader->data.size																			// SrcDataSize
		, (FileSystem::Get()->GetAppDirectory() + GetShaderFilenameWithPath(shader->name)).Str()	// pSourceName
		, defines																					// pDefines
		, D3D_COMPILE_STANDARD_FILE_INCLUDE															// pInclude
		, "main"																					// pEntrypoint
		, isVertexShader ? "vs_5_0" : "ps_5_0"														// pTarget
		, 0																							// Flags1
		, 0																							// Flags2
		, &renderData->shaderCode																	// ppCode
		, &errorBlob);																				// ppErrorMsgs
	if(FAILED(hr))
	{
		if(errorBlob != nullptr)
		{
			LOG_ERROR("Shader compile error: %s", (char*)errorBlob->GetBufferPointer());
			DX_RELEASE(errorBlob);
		}

		delete renderData;
		shader->renderData = isVertexShader ? errorVertexShader->renderData : errorPixelShader->renderData;
		return;
	}

	if(isVertexShader)
	{
//		hr = d3dDevice->CreateVertexShader(renderData->shaderCode->GetBufferPointer(), renderData->shaderCode->GetBufferSize(), nullptr, &renderData->vertexShader);
	}
	else
	{
//		hr = d3dDevice->CreatePixelShader(renderData->shaderCode->GetBufferPointer(), renderData->shaderCode->GetBufferSize(), nullptr, &renderData->pixelShader);
	}
	ASSERT(SUCCEEDED(hr));

//	renderData->ptr->SetPrivateData(WKPDID_D3DDebugObjectName, (UINT)shader->name.Length(), shader->name.Str());

	ID3D12ShaderReflection *reflection = nullptr;
	hr = D3DReflect(renderData->shaderCode->GetBufferPointer(), renderData->shaderCode->GetBufferSize() , IID_ID3D12ShaderReflection, (void**)&reflection);
	if(FAILED(hr))
	{
		LOG_ERROR("D3DReflect failed 0x%08x\n", (uint32_t)hr);
	}

	D3D12_SHADER_DESC shaderDesc;
	hr = reflection->GetDesc(&shaderDesc);
	if(FAILED(hr))
	{
		LOG_ERROR("ID3D12ShaderReflection::GetDesc failed 0x%08x\n", (uint32_t)hr);
	}

	renderData->constantBuffers.resize(shaderDesc.ConstantBuffers);
	renderData->constantBuffersData.resize(shaderDesc.ConstantBuffers);
/*	for(uint32_t i = 0; i < shaderDesc.ConstantBuffers; ++i)
	{
		ID3D12ShaderReflectionConstantBuffer *cbuffer = reflection->GetConstantBufferByIndex(i);
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
		ShaderRenderDataD3D12::ConstantBufferData& bufferData = renderData->constantBuffersData[i];
		bufferData.buffer = MemoryBuffer(shaderBufferDesc.Size);

		for(uint32_t j = 0; j < shaderBufferDesc.Variables; ++j)
		{
			ID3D12ShaderReflectionVariable *shaderVar = cbuffer->GetVariableByIndex(j);
			D3D12_SHADER_VARIABLE_DESC varDesc;
			hr = shaderVar->GetDesc(&varDesc);
			ASSERT(SUCCEEDED(hr));

			if((varDesc.uFlags & D3D_SVF_USED) == 0)
			{
				continue;
			}

			ID3D12ShaderReflectionType *type = shaderVar->GetType();
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

	DX_RELEASE(reflection);

	shader->renderData = renderData;
}

void Direct3D12Render::Bind(Texture *texture)
{
	ASSERT(texture != nullptr);

	if(texture->renderData != nullptr)
	{
		// Already bound
		return;
	}

	TextureRenderDataD3D12 *renderData = new TextureRenderDataD3D12();

	texture->renderData = renderData;
}

void Direct3D12Render::Bind(RenderTarget *renderTarget)
{
	ASSERT(renderTarget != nullptr);

	if(renderTarget->renderData != nullptr)
	{
		// Already bound
		return;
	}

	RenderTargetRenderDataD3D12 *renderData = new RenderTargetRenderDataD3D12();

	renderTarget->renderData = renderData;
}

void Direct3D12Render::Unbind(Mesh *mesh)
{
	if(mesh == nullptr || mesh->renderData == nullptr)
	{
		// Not yet bound
		return;
	}

	MeshRenderDataD3D12 *renderData = static_cast<MeshRenderDataD3D12*>(mesh->renderData);

	delete renderData;
	mesh->renderData = nullptr;

	if(activeMesh == mesh)
	{
		activeMesh = nullptr;
	}
}

void Direct3D12Render::Unbind(Shader *shader)
{
	if(shader == nullptr || shader->renderData == nullptr)
	{
		// Not yet bound
		return;
	}

	if((shader->renderData == errorVertexShader->renderData || shader->renderData == errorPixelShader->renderData) &&
		shader != errorVertexShader.get() &&
		shader != errorPixelShader.get())
	{
		shader->renderData = nullptr;
		return;
	}

	ShaderRenderDataD3D12 *renderData = static_cast<ShaderRenderDataD3D12*>(shader->renderData);

	delete renderData;
	shader->renderData = nullptr;

	if(activeVertexShader == shader)
	{
		activeVertexShader = nullptr;
	}
	if(activeFragmentShader == shader)
	{
		activeFragmentShader = nullptr;
	}
}

void Direct3D12Render::Unbind(Texture *texture)
{
	if(texture == nullptr || texture->renderData == nullptr)
	{
		// Not yet bound
		return;
	}

	TextureRenderDataD3D12 *renderData = static_cast<TextureRenderDataD3D12*>(texture->renderData);

	delete renderData;
	texture->renderData = nullptr;
}

void Direct3D12Render::Unbind(RenderTarget *renderTarget)
{
	if(renderTarget == nullptr || renderTarget->renderData == nullptr)
	{
		// Not yet bound
		return;
	}

	RenderTargetRenderDataD3D12 *renderData = static_cast<RenderTargetRenderDataD3D12*>(renderTarget->renderData);

	const uint8_t textureCount = std::min<uint8_t>(renderTarget->GetTextureCount(), D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT);
	for(uint8_t i = 0; i < textureCount; ++i)
	{
		Unbind(renderTarget->GetTexture(i).get());
	}

	delete renderData;
	renderTarget->renderData = nullptr;
}

void Direct3D12Render::UpdateDynamicMesh(DynamicMesh *mesh)
{
	if(mesh == nullptr || mesh->renderData == nullptr)
	{
		// Not yet bound
		mesh->isIndexDataUpdateRequired = false;
		mesh->isVertexDataUpdateRequired = false;
		return;
	}

	MeshRenderDataD3D12 *renderData = static_cast<MeshRenderDataD3D12*>(mesh->renderData);

	if(mesh->isIndexDataUpdateRequired)
	{
		mesh->isIndexDataUpdateRequired = false;
	}

	if(mesh->isVertexDataUpdateRequired)
	{
		mesh->isVertexDataUpdateRequired = false;
	}
}

void Direct3D12Render::SetViewport(uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
	const D3D12_VIEWPORT vp = { (float)x, (float)y, (float)width, (float)height, 0.0f, 1.0f };
}

void Direct3D12Render::SetMesh(Mesh *mesh)
{
	MeshRenderDataD3D12 *renderData = static_cast<MeshRenderDataD3D12*>(mesh->renderData);

	uint32_t indexByteOffset = renderData->indexOffset * sizeof(uint16_t);
	uint32_t vertexByteOffset = renderData->vertexOffset * mesh->stride;

	switch(mesh->type)
	{
		case Mesh::EType::Static:
		{
			if(activeMesh == mesh)
			{
				// No need to set the buffers again
				return;
			}
			break;
		}

		case Mesh::EType::Dynamic:
		{
			const DynamicMesh *dynamicMesh = static_cast<const DynamicMesh*>(mesh);
			indexByteOffset += dynamicMesh->indexOffset * sizeof(uint16_t);
			vertexByteOffset += dynamicMesh->vertexOffset * mesh->stride;
			break;
		}
	}

	activeMesh = mesh;
}

void Direct3D12Render::SetScreenSpaceQuadMesh()
{
	if(activeMesh == nullptr)
	{
		return;
	}
	
	activeMesh = nullptr;
}

void Direct3D12Render::SetVertexShader(Shader *vertexShader)
{
	if(activeVertexShader == vertexShader)
	{
		return;
	}

	const ShaderRenderDataD3D12 *vertexShaderRenderData = static_cast<const ShaderRenderDataD3D12*>(vertexShader->renderData);

	activeVertexShader = vertexShader;
}

void Direct3D12Render::SetFragmentShader(Shader *fragmentShader)
{
	if(activeFragmentShader == fragmentShader)
	{
		return;
	}

	const ShaderRenderDataD3D12 *shaderRenderData = static_cast<const ShaderRenderDataD3D12*>(fragmentShader->renderData);

	activeFragmentShader = fragmentShader;
}

void Direct3D12Render::SetTexture(uint8_t samplerIdx, Texture *texture, EFilterMode filterMode, EAddressMode addressMode)
{
	ASSERT(samplerIdx < D3D12_COMMONSHADER_SAMPLER_SLOT_COUNT);

	TextureRenderDataD3D12 *renderData = static_cast<TextureRenderDataD3D12*>(texture->renderData);

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
}

void Direct3D12Render::UpdateShaderParams(const Material *material)
{
	const ShaderRenderDataD3D12 *vertexShaderRenderData = static_cast<const ShaderRenderDataD3D12*>(activeVertexShader->renderData);
	UpdateShaderParams(material, vertexShaderRenderData);

	const ShaderRenderDataD3D12 *fragmentShaderRenderData = static_cast<const ShaderRenderDataD3D12*>(activeFragmentShader->renderData);
	UpdateShaderParams(material, fragmentShaderRenderData);
}

void Direct3D12Render::UpdateShaderParams(const Material *material, const ShaderRenderDataD3D12 *shaderRenderData)
{
	const std::pair<uint32_t, uint32_t> *offsetSizePair = nullptr;

	for(size_t i = 0; i < shaderRenderData->constantBuffers.size(); ++i)
	{
		bool isChanged = false;
		const ShaderRenderDataD3D12::ConstantBufferData& bufferData = shaderRenderData->constantBuffersData[i];

		for(const Material::ShaderParam& shaderParam : material->GetShaderParams())
		{
			offsetSizePair = bufferData.variableOffsetSizePairs.Find(shaderParam.name);
			if(offsetSizePair != nullptr)
			{
				isChanged |= CheckAndUpdateShaderParam(shaderParam.GetValue(), bufferData.buffer.data + offsetSizePair->first, offsetSizePair->second);
			}
		}

		offsetSizePair = bufferData.variableOffsetSizePairs.Find("matWorldView");
		if(offsetSizePair != nullptr && offsetSizePair->second == sizeof(DirectX::XMMATRIX))
		{
			const DirectX::XMMATRIX matWorldView = DirectX::XMMatrixMultiply(matWorld, matView);
			isChanged |= CheckAndUpdateShaderParam(&matWorldView.r[0], bufferData.buffer.data + offsetSizePair->first, offsetSizePair->second);
		}

		offsetSizePair = bufferData.variableOffsetSizePairs.Find("matWorldViewProj");
		if(offsetSizePair != nullptr && offsetSizePair->second == sizeof(DirectX::XMMATRIX))
		{
			const DirectX::XMMATRIX matWorldView = DirectX::XMMatrixMultiply(matWorld, matView);
			const DirectX::XMMATRIX matWorldViewProj = DirectX::XMMatrixMultiply(matWorldView, matProj);
			isChanged |= CheckAndUpdateShaderParam(&matWorldViewProj.r[0], bufferData.buffer.data + offsetSizePair->first, offsetSizePair->second);
		}

		offsetSizePair = bufferData.variableOffsetSizePairs.Find("matViewInv");
		if(offsetSizePair != nullptr)
		{
			const DirectX::XMMATRIX matViewInv = DirectX::XMMatrixInverse(nullptr, matView);
			isChanged |= CheckAndUpdateShaderParam(&matViewInv.r[0], bufferData.buffer.data + offsetSizePair->first, offsetSizePair->second);
		}

		offsetSizePair = bufferData.variableOffsetSizePairs.Find("camPos");
		if(offsetSizePair != nullptr)
		{
			const DirectX::XMMATRIX matViewInv = DirectX::XMMatrixInverse(nullptr, matView);
			isChanged |= CheckAndUpdateShaderParam(&matViewInv.r[3], bufferData.buffer.data + offsetSizePair->first, offsetSizePair->second);
		}

		offsetSizePair = bufferData.variableOffsetSizePairs.Find("resolution");
		if(offsetSizePair != nullptr && offsetSizePair->second == 2 * sizeof(float))
		{
			float resolution[2];
			if(activeView != nullptr)
			{
				resolution[0] = activeView->GetWidth();
				resolution[1] = activeView->GetHeight();
			}
			else
			{
				resolution[0] = activeWindow->GetWidth();
				resolution[1] = activeWindow->GetHeight();
			}

			isChanged |= CheckAndUpdateShaderParam(resolution, bufferData.buffer.data + offsetSizePair->first, offsetSizePair->second);
		}

		if(isChanged)
		{
//			deviceCtx->UpdateSubresource(shaderRenderData->constantBuffers[i], 0, nullptr, bufferData.buffer.data, 0, 0);
		}
	}
}

bool Direct3D12Render::CheckAndUpdateShaderParam(const void *value, void *valueInConstantBuffer, uint32_t size)
{
	if(memcmp(valueInConstantBuffer, value, size) == 0)
	{
		return false;
	}

	memcpy(valueInConstantBuffer, value, size);
	return true;
}

void Direct3D12Render::DoRender()
{

}

DXGI_FORMAT Direct3D12Render::GetTextureFormat(const Texture *texture)
{
	const DXGI_FORMAT conversionTable[] =
	{
		DXGI_FORMAT_UNKNOWN,					// Texture::EFormat::Unknown
		DXGI_FORMAT_R8_UNORM,					// Texture::EFormat::R8
		DXGI_FORMAT_R8G8_UNORM,					// Texture::EFormat::RG8
		DXGI_FORMAT_R8G8B8A8_UNORM,				// Texture::EFormat::RGB8
		DXGI_FORMAT_R8G8B8A8_UNORM,				// Texture::EFormat::RGBA8
		DXGI_FORMAT_R32G32B32A32_FLOAT,			// Texture::EFormat::RGBA32F
		DXGI_FORMAT_D16_UNORM,					// Texture::EFormat::D16
		DXGI_FORMAT_D24_UNORM_S8_UINT,			// Texture::EFormat::D24S8
		DXGI_FORMAT_D32_FLOAT,					// Texture::EFormat::D32
	};
	DESIRE_CHECK_ARRAY_SIZE(conversionTable, Texture::EFormat::D32 + 1);

	return conversionTable[(size_t)texture->format];
}
