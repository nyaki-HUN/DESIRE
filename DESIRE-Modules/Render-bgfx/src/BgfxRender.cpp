#include "stdafx_bgfx.h"
#include "BgfxRender.h"

#include "Embedded_shaders/vs_screenSpaceQuad.bin.h"
#include "MeshRenderDataBgfx.h"
#include "RenderTargetRenderDataBgfx.h"
#include "ShaderRenderDataBgfx.h"
#include "TextureRenderDataBgfx.h"

#include "Engine/Application/OSWindow.h"

#include "Engine/Core/Math/Matrix4.h"
#include "Engine/Core/String/StackString.h"

#include "Engine/Render/Material.h"
#include "Engine/Render/RenderTarget.h"
#include "Engine/Render/View.h"

#include "Engine/Resource/Mesh.h"
#include "Engine/Resource/Shader.h"
#include "Engine/Resource/Texture.h"

#include "bgfx/../../src/config.h"

static const bgfx::EmbeddedShader s_embeddedShaders[] =
{
	BGFX_EMBEDDED_SHADER(vs_screenSpaceQuad),
	BGFX_EMBEDDED_SHADER_END()
};

BgfxRender::BgfxRender()
{
	for(bgfx::UniformHandle& uniform : samplerUniforms)
	{
		uniform = BGFX_INVALID_HANDLE;
	}

	screenSpaceQuadVertexLayout.begin();
	screenSpaceQuadVertexLayout.add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float);
	screenSpaceQuadVertexLayout.end();
}

void BgfxRender::Init(OSWindow* mainWindow)
{
	bgfx::PlatformData pd = {};
#if DESIRE_PLATFORM_LINUX
	pd.ndt = GetDisplay();
#endif	// #if DESIRE_PLATFORM_LINUX
	pd.nwh = mainWindow->GetHandle();
	bgfx::setPlatformData(pd);

	bgfx::Init initParams;
	initParams.resolution.format = bgfx::TextureFormat::RGBA8;
	initParams.resolution.width = mainWindow->GetWidth();
	initParams.resolution.height = mainWindow->GetHeight();
	initialized = bgfx::init(initParams);
	activeViewId = 0;

	for(bgfx::ViewId viewId = 0; viewId < BGFX_CONFIG_MAX_VIEWS; ++viewId)
	{
		bgfx::setViewClear(viewId, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH | BGFX_CLEAR_STENCIL, clearColor, 1.0f, 0);
	}

	samplerUniforms[0] = bgfx::createUniform("s_tex", bgfx::UniformType::Sampler);
	for(uint8_t i = 1; i < DESIRE_ASIZEOF(samplerUniforms); ++i)
	{
		samplerUniforms[i] = bgfx::createUniform(StackString<7>::Format("s_tex%u", i).Str(), bgfx::UniformType::Sampler);
	}

	renderState = BGFX_STATE_MSAA;
	SetDefaultRenderStates();

	BindEmbeddedShader(screenSpaceQuadVertexShader.get());

	bgfx::reset(mainWindow->GetWidth(), mainWindow->GetHeight(), BGFX_RESET_VSYNC);
}

void BgfxRender::UpdateRenderWindow(OSWindow* window)
{
	if(!initialized)
	{
		return;
	}

	bgfx::reset(window->GetWidth(), window->GetHeight(), BGFX_RESET_VSYNC);
}

void BgfxRender::Kill()
{
	for(bgfx::UniformHandle& uniform : samplerUniforms)
	{
		bgfx::destroy(uniform);
		uniform = BGFX_INVALID_HANDLE;
	}

	for(auto& pair : shaderProgramCache)
	{
		bgfx::destroy(pair.second);
	}
	shaderProgramCache.clear();

	pActiveVertexShader = nullptr;
	pActiveFragmentShader = nullptr;

	Unbind(screenSpaceQuadVertexShader.get());

	bgfx::shutdown();
	initialized = false;
}

void BgfxRender::AppendShaderFilenameWithPath(WritableString& outString, const String& shaderFilename) const
{
	outString += "data/shaders/bgfx/";

	switch(bgfx::getRendererType())
	{
		case bgfx::RendererType::Direct3D9:		outString += "dx9/"; break;
		case bgfx::RendererType::Direct3D11:	outString += "dx11/"; break;
		case bgfx::RendererType::Direct3D12:	outString += "dx11/"; break;
		case bgfx::RendererType::Gnm:			outString += "pssl/"; break;
		case bgfx::RendererType::Metal:			outString += "metal/"; break;
		case bgfx::RendererType::OpenGLES:		outString += "essl/"; break;
		case bgfx::RendererType::OpenGL:		outString += "glsl/"; break;
		case bgfx::RendererType::Vulkan:		outString += "spirv/"; break;
		default:								ASSERT(false && "Not supported renderer type"); break;
	}

	outString += shaderFilename;
	outString += ".bin";
}

void BgfxRender::BeginFrame(OSWindow* window)
{
	activeViewId = 0;
	SetViewport(0, 0, window->GetWidth(), window->GetHeight());

	// This dummy draw call is here to make sure that view 0 is cleared if no other draw calls are submitted to view 0
	bgfx::touch(0);
}

void BgfxRender::EndFrame()
{
	bgfx::frame();
}

void BgfxRender::SetView(View* view)
{
	if(view != nullptr)
	{
		RenderTarget* rt = view->GetRenderTarget();
		if(rt->pRenderData == nullptr)
		{
			Bind(rt);
		}

		RenderTargetRenderDataBgfx* pRenderTargetRenderData = static_cast<RenderTargetRenderDataBgfx*>(rt->pRenderData);
		ASSERT(pRenderTargetRenderData->id < BGFX_CONFIG_MAX_VIEWS);
		activeViewId = pRenderTargetRenderData->id;
		SetViewport(view->GetPosX(), view->GetPosY(), view->GetWidth(), view->GetHeight());

		bgfx::setViewClear(activeViewId, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH | BGFX_CLEAR_STENCIL, clearColor, 1.0f, 0);
	}
	else
	{
		activeViewId = 0;
	}
}

void BgfxRender::SetWorldMatrix(const Matrix4& matrix)
{
	float world[16];
	matrix.Store(world);

	bgfx::setTransform(world);
}

void BgfxRender::SetViewProjectionMatrices(const Matrix4& viewMatrix, const Matrix4& projMatrix)
{
	float view[16];
	float projection[16];
	viewMatrix.Store(view);
	projMatrix.Store(projection);

	bgfx::setViewTransform(activeViewId, view, projection);
}

void BgfxRender::SetScissor(uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
	bgfx::setScissor(x, y, width, height);
}

void BgfxRender::SetClearColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	clearColor = (r << 24) | (g << 16) | (b << 8) | a;
}

void BgfxRender::SetColorWriteEnabled(bool r, bool g, bool b, bool a)
{
	if(r)
	{
		renderState |= BGFX_STATE_WRITE_R;
	}
	else
	{
		renderState &= ~BGFX_STATE_WRITE_R;
	}

	if(g)
	{
		renderState |= BGFX_STATE_WRITE_G;
	}
	else
	{
		renderState &= ~BGFX_STATE_WRITE_G;
	}

	if(b)
	{
		renderState |= BGFX_STATE_WRITE_B;
	}
	else
	{
		renderState &= ~BGFX_STATE_WRITE_B;
	}

	if(a)
	{
		renderState |= BGFX_STATE_WRITE_A;
	}
	else
	{
		renderState &= ~BGFX_STATE_WRITE_A;
	}
}

void BgfxRender::SetDepthWriteEnabled(bool enabled)
{
	if(enabled)
	{
		renderState |= BGFX_STATE_WRITE_Z;
	}
	else
	{
		renderState &= ~BGFX_STATE_WRITE_Z;
	}
}

void BgfxRender::SetDepthTest(EDepthTest depthTest)
{
	renderState &= ~BGFX_STATE_DEPTH_TEST_MASK;

	switch(depthTest)
	{
		case EDepthTest::Disabled:		break;
		case EDepthTest::Less:			renderState |= BGFX_STATE_DEPTH_TEST_LESS; break;
		case EDepthTest::LessEqual:		renderState |= BGFX_STATE_DEPTH_TEST_LEQUAL; break;
		case EDepthTest::Greater:		renderState |= BGFX_STATE_DEPTH_TEST_GREATER; break;
		case EDepthTest::GreaterEqual:	renderState |= BGFX_STATE_DEPTH_TEST_GEQUAL; break;
		case EDepthTest::Equal:			renderState |= BGFX_STATE_DEPTH_TEST_EQUAL; break;
		case EDepthTest::NotEqual:		renderState |= BGFX_STATE_DEPTH_TEST_NOTEQUAL; break;
	}
}

void BgfxRender::SetCullMode(ECullMode cullMode)
{
	renderState &= ~BGFX_STATE_CULL_MASK;

	switch(cullMode)
	{
		case ECullMode::None:	break;
		case ECullMode::CCW:	renderState |= BGFX_STATE_CULL_CCW; break;
		case ECullMode::CW:		renderState |= BGFX_STATE_CULL_CW; break;
	}
}

void BgfxRender::SetBlendModeSeparated(EBlend srcBlendRGB, EBlend destBlendRGB, EBlendOp blendOpRGB, EBlend srcBlendAlpha, EBlend destBlendAlpha, EBlendOp blendOpAlpha)
{
	renderState &= ~BGFX_STATE_BLEND_MASK;

	static const uint64_t s_blendConversionTable[] =
	{
		BGFX_STATE_BLEND_ZERO,				// EBlend::Zero
		BGFX_STATE_BLEND_ONE,				// EBlend::One
		BGFX_STATE_BLEND_SRC_COLOR,			// EBlend::SrcColor
		BGFX_STATE_BLEND_INV_SRC_COLOR,		// EBlend::InvSrcColor
		BGFX_STATE_BLEND_SRC_ALPHA,			// EBlend::SrcAlpha
		BGFX_STATE_BLEND_INV_SRC_ALPHA,		// EBlend::InvSrcAlpha
		BGFX_STATE_BLEND_DST_ALPHA,			// EBlend::DestAlpha
		BGFX_STATE_BLEND_INV_DST_ALPHA,		// EBlend::InvDestAlpha
		BGFX_STATE_BLEND_DST_COLOR,			// EBlend::DestColor
		BGFX_STATE_BLEND_INV_DST_COLOR,		// EBlend::InvDestColor
		BGFX_STATE_BLEND_SRC_ALPHA_SAT,		// EBlend::SrcAlphaSat
		BGFX_STATE_BLEND_FACTOR,			// EBlend::BlendFactor
		BGFX_STATE_BLEND_INV_FACTOR			// EBlend::InvBlendFactor
	};
	DESIRE_CHECK_ARRAY_SIZE(s_blendConversionTable, EBlend::InvBlendFactor + 1);

	const uint64_t srcRGB = s_blendConversionTable[(size_t)srcBlendRGB];
	const uint64_t destRGB = s_blendConversionTable[(size_t)destBlendRGB];
	const uint64_t srcAlpha = s_blendConversionTable[(size_t)srcBlendAlpha];
	const uint64_t destAlpha = s_blendConversionTable[(size_t)destBlendAlpha];
	renderState |= BGFX_STATE_BLEND_FUNC_SEPARATE(srcRGB, destRGB, srcAlpha, destAlpha);

	static const uint64_t s_equationConversionTable[] =
	{
		BGFX_STATE_BLEND_EQUATION_ADD,		// EBlendOp::Add
		BGFX_STATE_BLEND_EQUATION_SUB,		// EBlendOp::Subtract
		BGFX_STATE_BLEND_EQUATION_REVSUB,	// EBlendOp::RevSubtract
		BGFX_STATE_BLEND_EQUATION_MIN,		// EBlendOp::Min
		BGFX_STATE_BLEND_EQUATION_MAX		// EBlendOp::Max
	};
	DESIRE_CHECK_ARRAY_SIZE(s_equationConversionTable, EBlendOp::Max + 1);

	const uint64_t equationRGB = s_equationConversionTable[(size_t)blendOpRGB];
	const uint64_t equationAlpha = s_equationConversionTable[(size_t)blendOpAlpha];
	renderState |= BGFX_STATE_BLEND_EQUATION_SEPARATE(equationRGB, equationAlpha);
}

void BgfxRender::SetBlendModeDisabled()
{
	renderState &= ~BGFX_STATE_BLEND_MASK;
}

void* BgfxRender::CreateMeshRenderData(const Mesh* pMesh)
{
	MeshRenderDataBgfx* pMeshRenderData = new MeshRenderDataBgfx();

	static const bgfx::Attrib::Enum s_attribConversionTable[] =
	{
		bgfx::Attrib::Position,		// Mesh::EAttrib::Position
		bgfx::Attrib::Normal,		// Mesh::EAttrib::Normal
		bgfx::Attrib::Color0,		// Mesh::EAttrib::Color
		bgfx::Attrib::TexCoord0,	// Mesh::EAttrib::Texcoord0
		bgfx::Attrib::TexCoord1,	// Mesh::EAttrib::Texcoord1
		bgfx::Attrib::TexCoord2,	// Mesh::EAttrib::Texcoord2
		bgfx::Attrib::TexCoord3,	// Mesh::EAttrib::Texcoord3
		bgfx::Attrib::TexCoord4,	// Mesh::EAttrib::Texcoord4
		bgfx::Attrib::TexCoord5,	// Mesh::EAttrib::Texcoord5
		bgfx::Attrib::TexCoord6,	// Mesh::EAttrib::Texcoord6
		bgfx::Attrib::TexCoord7,	// Mesh::EAttrib::Texcoord7
	};
	DESIRE_CHECK_ARRAY_SIZE(s_attribConversionTable, Mesh::EAttrib::Num);

	static const bgfx::AttribType::Enum s_attribTypeConversionTable[] =
	{
		bgfx::AttribType::Enum::Float,	// Mesh::EAttribType::Float
		bgfx::AttribType::Enum::Uint8,	// Mesh::EAttribType::Uint8
	};
	DESIRE_CHECK_ARRAY_SIZE(s_attribTypeConversionTable, Mesh::EAttribType::Num);

	pMeshRenderData->vertexLayout.begin();
	const Array<Mesh::VertexLayout>& vertexLayout = pMesh->GetVertexLayout();
	for(const Mesh::VertexLayout& layout : vertexLayout)
	{
		const bool isNormalized = (layout.type == Mesh::EAttribType::Uint8);
		pMeshRenderData->vertexLayout.add(s_attribConversionTable[(size_t)layout.attrib], layout.count, s_attribTypeConversionTable[(size_t)layout.type], isNormalized);
	}
	pMeshRenderData->vertexLayout.end();

	const bgfx::Memory* pIndexData = (pMesh->numIndices != 0) ? bgfx::copy(pMesh->indices.get(), pMesh->GetSizeOfIndexData()) : nullptr;
	const bgfx::Memory* pVertexData = bgfx::copy(pMesh->vertices.get(), pMesh->GetSizeOfVertexData());

	switch(pMesh->GetType())
	{
		case Mesh::EType::Static:
			if(pIndexData != nullptr)
			{
				pMeshRenderData->indexBuffer = bgfx::createIndexBuffer(pIndexData, BGFX_BUFFER_NONE);
			}

			pMeshRenderData->vertexBuffer = bgfx::createVertexBuffer(pVertexData, pMeshRenderData->vertexLayout, BGFX_BUFFER_NONE);
			break;

		case Mesh::EType::Dynamic:
			if(pIndexData != nullptr)
			{
				pMeshRenderData->dynamicIndexBuffer = bgfx::createDynamicIndexBuffer(pIndexData, BGFX_BUFFER_NONE);
			}

			pMeshRenderData->dynamicVertexBuffer = bgfx::createDynamicVertexBuffer(pVertexData, pMeshRenderData->vertexLayout, BGFX_BUFFER_NONE);
			break;
	}

	return pMeshRenderData;
}

void* BgfxRender::CreateShaderRenderData(const Shader* pShader)
{
	ShaderRenderDataBgfx* pShaderRenderData = new ShaderRenderDataBgfx();

	const bgfx::Memory* shaderData = nullptr;
	if(pShader->defines.empty())
	{
		shaderData = bgfx::makeRef(pShader->data.ptr.get(), static_cast<uint32_t>(pShader->data.size));
	}
	else
	{
		size_t totalDefinesLength = 0;
		for(const String& define : pShader->defines)
		{
			totalDefinesLength += 8 + define.Length() + 3;	// "#define ASD 1\n"
		}

		shaderData = bgfx::alloc(static_cast<uint32_t>(totalDefinesLength + pShader->data.size));
		uint8_t* ptr = shaderData->data;
		for(const String& define : pShader->defines)
		{
			memcpy(ptr, "#define ", 8);
			ptr += 8;
			memcpy(ptr, define.Str(), define.Length());
			ptr += define.Length();
			memcpy(ptr, " 1\n", 3);
			ptr += 3;
		}
		memcpy(ptr, pShader->data.ptr.get(), pShader->data.size);
	}

	pShaderRenderData->shaderHandle = bgfx::createShader(shaderData);
	ASSERT(bgfx::isValid(pShaderRenderData->shaderHandle));
	bgfx::setName(pShaderRenderData->shaderHandle, pShader->name.Str());

	const uint16_t uniformCount = bgfx::getShaderUniforms(pShaderRenderData->shaderHandle, nullptr, 0);
	bgfx::UniformHandle* uniforms = static_cast<bgfx::UniformHandle*>(alloca(uniformCount * sizeof(bgfx::UniformHandle)));
	bgfx::getShaderUniforms(pShaderRenderData->shaderHandle, uniforms, uniformCount);
	for(uint16_t i = 0; i < uniformCount; ++i)
	{
		bool isSampler = false;
		for(bgfx::UniformHandle& samplerUniform : samplerUniforms)
		{
			if(uniforms[i].idx == samplerUniform.idx)
			{
				isSampler = true;
				break;
			}
		}

		if(isSampler)
		{
			// Skip sampler uniforms
			continue;
		}

		bgfx::UniformInfo info;
		bgfx::getUniformInfo(uniforms[i], info);
		pShaderRenderData->uniforms.Insert(HashedString::CreateFromString(info.name), uniforms[i]);
	}

	return pShaderRenderData;
}

void* BgfxRender::CreateTextureRenderData(const Texture* pTexture)
{
	TextureRenderDataBgfx* pTextureRenderData = new TextureRenderDataBgfx();

	const bool isRenderTarget = (pTexture->data == nullptr);

	if(isRenderTarget)
	{
		pTextureRenderData->textureHandle = bgfx::createTexture2D(pTexture->width, pTexture->height, (pTexture->numMipLevels > 1), 1, GetTextureFormat(pTexture), BGFX_TEXTURE_RT);
	}
	else
	{
		pTextureRenderData->textureHandle = bgfx::createTexture2D(pTexture->width, pTexture->height, (pTexture->numMipLevels > 1), 1, GetTextureFormat(pTexture), BGFX_TEXTURE_NONE, bgfx::makeRef(pTexture->data.get(), pTexture->GetDataSize()));
	}

	return pTextureRenderData;
}

void* BgfxRender::CreateRenderTargetRenderData(const RenderTarget* pRenderTarget)
{
	RenderTargetRenderDataBgfx* pRenderTargetRenderData = new RenderTargetRenderDataBgfx();

	bgfx::TextureHandle renderTargetTextures[BGFX_CONFIG_MAX_FRAME_BUFFER_ATTACHMENTS];
	const uint8_t textureCount = std::min<uint8_t>(pRenderTarget->GetTextureCount(), BGFX_CONFIG_MAX_FRAME_BUFFER_ATTACHMENTS);
	for(uint8_t i = 0; i < textureCount; ++i)
	{
		const std::shared_ptr<Texture>& texture = pRenderTarget->GetTexture(i);
		const TextureRenderDataBgfx* pTextureRenderData = static_cast<const TextureRenderDataBgfx*>(texture->pRenderData);
		ASSERT(pTextureRenderData != nullptr);

		renderTargetTextures[i] = pTextureRenderData->textureHandle;
	}

	pRenderTargetRenderData->frameBuffer = bgfx::createFrameBuffer(textureCount, renderTargetTextures);
	bgfx::setViewFrameBuffer(pRenderTargetRenderData->id, pRenderTargetRenderData->frameBuffer);

	return pRenderTargetRenderData;
}

void BgfxRender::DestroyMeshRenderData(void* pRenderData)
{
	MeshRenderDataBgfx* pMeshRenderData = static_cast<MeshRenderDataBgfx*>(pRenderData);

	bgfx::destroy(pMeshRenderData->indexBuffer);
	bgfx::destroy(pMeshRenderData->vertexBuffer);

	bgfx::destroy(pMeshRenderData->dynamicIndexBuffer);
	bgfx::destroy(pMeshRenderData->dynamicVertexBuffer);

	delete pRenderData;
}

void BgfxRender::DestroyShaderRenderData(void* pRenderData)
{
	ShaderRenderDataBgfx* pShaderRenderData = static_cast<ShaderRenderDataBgfx*>(pRenderData);
	bgfx::destroy(pShaderRenderData->shaderHandle);
	delete pShaderRenderData;
}

void BgfxRender::DestroyTextureRenderData(void* pRenderData)
{
	TextureRenderDataBgfx* pTextureRenderData = static_cast<TextureRenderDataBgfx*>(pRenderData);
	bgfx::destroy(pTextureRenderData->textureHandle);
	delete pTextureRenderData;
}

void BgfxRender::DestroyRenderTargetRenderData(void* pRenderData)
{
	RenderTargetRenderDataBgfx* pRenderTargetRenderData = static_cast<RenderTargetRenderDataBgfx*>(pRenderData);
	bgfx::destroy(pRenderTargetRenderData->frameBuffer);
	delete pRenderTargetRenderData;
}

void BgfxRender::SetViewport(uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
	bgfx::setViewRect(activeViewId, x, y, width, height);
}

void BgfxRender::SetMesh(Mesh* pMesh)
{
	// Nothing to do
	DESIRE_UNUSED(pMesh);
}

void BgfxRender::UpdateDynamicMesh(DynamicMesh& dynamicMesh)
{
	MeshRenderDataBgfx* pMeshRenderData = static_cast<MeshRenderDataBgfx*>(dynamicMesh.pRenderData);

	if(dynamicMesh.isIndicesDirty)
	{
		bgfx::update(pMeshRenderData->dynamicIndexBuffer, 0, bgfx::copy(dynamicMesh.indices.get(), dynamicMesh.GetSizeOfIndexData()));
		dynamicMesh.isIndicesDirty = false;
	}

	if(dynamicMesh.isVerticesDirty)
	{
		bgfx::update(pMeshRenderData->dynamicVertexBuffer, 0, bgfx::copy(dynamicMesh.vertices.get(), dynamicMesh.GetSizeOfVertexData()));
		dynamicMesh.isVerticesDirty = false;
	}
}

void BgfxRender::SetVertexShader(Shader* vertexShader)
{
	pActiveVertexShader = vertexShader;
}

void BgfxRender::SetFragmentShader(Shader* fragmentShader)
{
	pActiveFragmentShader = fragmentShader;
}

void BgfxRender::SetTexture(uint8_t samplerIdx, const Texture& texture, EFilterMode filterMode, EAddressMode addressMode)
{
	uint32_t flags = BGFX_TEXTURE_NONE;
	switch(filterMode)
	{
		case EFilterMode::Point:		flags |= BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT | BGFX_SAMPLER_MIP_POINT; break;
		case EFilterMode::Bilinear:		flags |= BGFX_SAMPLER_MIP_POINT; break;
		case EFilterMode::Trilinear:	break;
		case EFilterMode::Anisotropic:	flags |= BGFX_SAMPLER_MIN_ANISOTROPIC | BGFX_SAMPLER_MAG_ANISOTROPIC; break;
	}
	switch(addressMode)
	{
		case EAddressMode::Repeat:			break;
		case EAddressMode::Clamp:			flags |= BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_W_CLAMP; break;
		case EAddressMode::MirroredRepeat:	flags |= BGFX_SAMPLER_U_MIRROR | BGFX_SAMPLER_V_MIRROR | BGFX_SAMPLER_W_MIRROR; break;
		case EAddressMode::MirrorOnce:		flags |= BGFX_SAMPLER_U_MIRROR | BGFX_SAMPLER_V_MIRROR | BGFX_SAMPLER_W_MIRROR; break;
		case EAddressMode::Border:			flags |= BGFX_SAMPLER_U_BORDER | BGFX_SAMPLER_V_BORDER | BGFX_SAMPLER_W_BORDER; break;
	}

	const TextureRenderDataBgfx* pTextureRenderData = static_cast<const TextureRenderDataBgfx*>(texture.pRenderData);
	bgfx::setTexture(samplerIdx, samplerUniforms[samplerIdx], pTextureRenderData->textureHandle, flags);
}

void BgfxRender::UpdateShaderParams(const Material* material)
{
	const ShaderRenderDataBgfx* vertexShaderRenderData = static_cast<const ShaderRenderDataBgfx*>(pActiveVertexShader->pRenderData);
	const ShaderRenderDataBgfx* fragmentShaderRenderData = static_cast<const ShaderRenderDataBgfx*>(pActiveFragmentShader->pRenderData);

	for(const Material::ShaderParam& shaderParam : material->GetShaderParams())
	{
		const bgfx::UniformHandle* uniform = vertexShaderRenderData->uniforms.Find(shaderParam.name);
		if(uniform == nullptr)
		{
			uniform = fragmentShaderRenderData->uniforms.Find(shaderParam.name);
		}

		if(uniform != nullptr && bgfx::isValid(*uniform))
		{
			const void* value = shaderParam.GetValue();
			bgfx::setUniform(*uniform, value);
		}
	}
}

void BgfxRender::DoRender(uint32_t indexOffset, uint32_t vertexOffset, uint32_t numIndices, uint32_t numVertices)
{
	bgfx::setState(renderState, blendFactor);

	bgfx::ProgramHandle shaderProgram = BGFX_INVALID_HANDLE;

	const std::pair<uint64_t, uint64_t> key = std::make_pair(reinterpret_cast<uint64_t>(pActiveVertexShader->pRenderData), reinterpret_cast<uint64_t>(pActiveFragmentShader->pRenderData));
	auto it = shaderProgramCache.find(key);
	if(it != shaderProgramCache.end())
	{
		shaderProgram = it->second;
	}
	else
	{
		const ShaderRenderDataBgfx* pVertexShaderRenderData = static_cast<ShaderRenderDataBgfx*>(pActiveVertexShader->pRenderData);
		const ShaderRenderDataBgfx* pFragmentShaderRenderData = static_cast<const ShaderRenderDataBgfx*>(pActiveFragmentShader->pRenderData);
		shaderProgram = bgfx::createProgram(pVertexShaderRenderData->shaderHandle, pFragmentShaderRenderData->shaderHandle);

		shaderProgramCache.insert(std::make_pair(key, shaderProgram));
	}

	if(pActiveMesh != nullptr)
	{
		MeshRenderDataBgfx* pMeshRenderData = static_cast<MeshRenderDataBgfx*>(pActiveMesh->pRenderData);

		switch(pActiveMesh->GetType())
		{
			case Mesh::EType::Static:
			{
				bgfx::setIndexBuffer(pMeshRenderData->indexBuffer, pMeshRenderData->indexOffset + indexOffset, numIndices);
				bgfx::setVertexBuffer(0, pMeshRenderData->vertexBuffer, pMeshRenderData->vertexOffset + vertexOffset, numVertices);
				break;
			}

			case Mesh::EType::Dynamic:
			{
				bgfx::setIndexBuffer(pMeshRenderData->dynamicIndexBuffer, pMeshRenderData->indexOffset + indexOffset, numIndices);
				bgfx::setVertexBuffer(0, pMeshRenderData->dynamicVertexBuffer, pMeshRenderData->vertexOffset + vertexOffset, numVertices);
				break;
			}
		}
	}
	else
	{
		// Set screen space quad
		bgfx::TransientIndexBuffer indexBuffer;
		bgfx::TransientVertexBuffer vertexBuffer;
		if(bgfx::allocTransientBuffers(&vertexBuffer, screenSpaceQuadVertexLayout, 4, &indexBuffer, 6))
		{
			// Vertices
			const float vertices[] =
			{
				-1.0f,	 1.0f,
				1.0f,	 1.0f,
				-1.0f,	-1.0f,
				1.0f,	-1.0f
			};
			memcpy(vertexBuffer.data, vertices, sizeof(vertices));

			// Indices
			uint16_t* pIndices = reinterpret_cast<uint16_t*>(indexBuffer.data);
			pIndices[0] = 0;
			pIndices[1] = 1;
			pIndices[2] = 2;

			pIndices[3] = 3;
			pIndices[4] = 2;
			pIndices[5] = 1;
		}

		bgfx::setIndexBuffer(&indexBuffer);
		bgfx::setVertexBuffer(0, &vertexBuffer);
	}

	bgfx::submit(activeViewId, shaderProgram);
}

bgfx::TextureFormat::Enum BgfxRender::GetTextureFormat(const Texture* texture)
{
	const bgfx::TextureFormat::Enum conversionTable[] =
	{
		bgfx::TextureFormat::R8,			// Texture::EFormat::R8
		bgfx::TextureFormat::RG8,			// Texture::EFormat::RG8
		bgfx::TextureFormat::RGB8,			// Texture::EFormat::RGB8
		bgfx::TextureFormat::RGBA8,			// Texture::EFormat::RGBA8
		bgfx::TextureFormat::Unknown,		// Texture::EFormat::RGB32F
		bgfx::TextureFormat::RGBA32F,		// Texture::EFormat::RGBA32F
		bgfx::TextureFormat::D16,			// Texture::EFormat::D16
		bgfx::TextureFormat::D24S8,			// Texture::EFormat::D24_S8
		bgfx::TextureFormat::D32,			// Texture::EFormat::D32
	};
	DESIRE_CHECK_ARRAY_SIZE(conversionTable, Texture::EFormat::D32 + 1);

	return conversionTable[static_cast<size_t>(texture->format)];
}

void BgfxRender::BindEmbeddedShader(Shader* pShader)
{
	ASSERT(pShader != nullptr && pShader->pRenderData == nullptr);

	ShaderRenderDataBgfx* pShaderRenderData = new ShaderRenderDataBgfx();
	pShaderRenderData->shaderHandle = bgfx::createEmbeddedShader(s_embeddedShaders, bgfx::getRendererType(), pShader->name.Str());
	ASSERT(bgfx::isValid(pShaderRenderData->shaderHandle));

	pShader->pRenderData = pShaderRenderData;
}
