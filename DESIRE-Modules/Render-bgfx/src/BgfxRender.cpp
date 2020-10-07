#include "stdafx_bgfx.h"
#include "BgfxRender.h"

#include "MeshRenderDataBgfx.h"
#include "RenderableRenderDataBgfx.h"
#include "RenderTargetRenderDataBgfx.h"
#include "ShaderRenderDataBgfx.h"
#include "TextureRenderDataBgfx.h"

#include "Engine/Application/OSWindow.h"

#include "Engine/Core/Math/Matrix4.h"
#include "Engine/Core/String/StackString.h"

#include "Engine/Render/Material.h"
#include "Engine/Render/Mesh.h"
#include "Engine/Render/Renderable.h"
#include "Engine/Render/RenderTarget.h"
#include "Engine/Render/Shader.h"
#include "Engine/Render/Texture.h"

#include "bgfx/../../src/config.h"

BgfxRender::BgfxRender()
{
	for(bgfx::UniformHandle& uniform : m_samplerUniforms)
	{
		uniform = BGFX_INVALID_HANDLE;
	}
}

bool BgfxRender::Init(OSWindow& mainWindow)
{
	bgfx::PlatformData pd = {};
#if DESIRE_PLATFORM_LINUX
	pd.ndt = GetDisplay();
#endif	// #if DESIRE_PLATFORM_LINUX
	pd.nwh = mainWindow.GetHandle();
	bgfx::setPlatformData(pd);

	bgfx::Init initParams;
	initParams.resolution.format = bgfx::TextureFormat::RGBA8;
	initParams.resolution.width = mainWindow.GetWidth();
	initParams.resolution.height = mainWindow.GetHeight();
	const bool isInitialized = bgfx::init(initParams);
	if(!isInitialized)
	{
		return false;
	}

	m_activeViewId = 0;

	m_samplerUniforms[0] = bgfx::createUniform("s_tex", bgfx::UniformType::Sampler);
	for(uint8_t i = 1; i < DESIRE_ASIZEOF(m_samplerUniforms); ++i)
	{
		m_samplerUniforms[i] = bgfx::createUniform(StackString<7>::Format("s_tex%u", i).Str(), bgfx::UniformType::Sampler);
	}

	m_renderState = BGFX_STATE_MSAA;
	SetDefaultRenderStates();

	bgfx::reset(mainWindow.GetWidth(), mainWindow.GetHeight(), BGFX_RESET_VSYNC);

	return true;
}

void BgfxRender::UpdateRenderWindow(OSWindow& window)
{
	bgfx::reset(window.GetWidth(), window.GetHeight(), BGFX_RESET_VSYNC);
}

void BgfxRender::Kill()
{
	m_pActiveWindow = nullptr;
	m_pActiveMesh = nullptr;
	m_pActiveRenderTarget = nullptr;

	for(bgfx::UniformHandle& uniform : m_samplerUniforms)
	{
		bgfx::destroy(uniform);
		uniform = BGFX_INVALID_HANDLE;
	}

	bgfx::shutdown();
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

void BgfxRender::EndFrame()
{
	bgfx::frame();
}

void BgfxRender::Clear(uint32_t clearColorRGBA, float depth, uint8_t stencil)
{
	bgfx::setViewClear(m_activeViewId, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH | BGFX_CLEAR_STENCIL, clearColorRGBA, depth, stencil);
	bgfx::touch(m_activeViewId);
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

	bgfx::setViewTransform(m_activeViewId, view, projection);
}

void BgfxRender::SetScissor(uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
	bgfx::setScissor(x, y, width, height);
}

void BgfxRender::SetColorWriteEnabled(bool r, bool g, bool b, bool a)
{
	if(r)
	{
		m_renderState |= BGFX_STATE_WRITE_R;
	}
	else
	{
		m_renderState &= ~BGFX_STATE_WRITE_R;
	}

	if(g)
	{
		m_renderState |= BGFX_STATE_WRITE_G;
	}
	else
	{
		m_renderState &= ~BGFX_STATE_WRITE_G;
	}

	if(b)
	{
		m_renderState |= BGFX_STATE_WRITE_B;
	}
	else
	{
		m_renderState &= ~BGFX_STATE_WRITE_B;
	}

	if(a)
	{
		m_renderState |= BGFX_STATE_WRITE_A;
	}
	else
	{
		m_renderState &= ~BGFX_STATE_WRITE_A;
	}
}

void BgfxRender::SetDepthWriteEnabled(bool enabled)
{
	if(enabled)
	{
		m_renderState |= BGFX_STATE_WRITE_Z;
	}
	else
	{
		m_renderState &= ~BGFX_STATE_WRITE_Z;
	}
}

void BgfxRender::SetDepthTest(EDepthTest depthTest)
{
	m_renderState &= ~BGFX_STATE_DEPTH_TEST_MASK;

	switch(depthTest)
	{
		case EDepthTest::Disabled:		break;
		case EDepthTest::Less:			m_renderState |= BGFX_STATE_DEPTH_TEST_LESS; break;
		case EDepthTest::LessEqual:		m_renderState |= BGFX_STATE_DEPTH_TEST_LEQUAL; break;
		case EDepthTest::Greater:		m_renderState |= BGFX_STATE_DEPTH_TEST_GREATER; break;
		case EDepthTest::GreaterEqual:	m_renderState |= BGFX_STATE_DEPTH_TEST_GEQUAL; break;
		case EDepthTest::Equal:			m_renderState |= BGFX_STATE_DEPTH_TEST_EQUAL; break;
		case EDepthTest::NotEqual:		m_renderState |= BGFX_STATE_DEPTH_TEST_NOTEQUAL; break;
	}
}

void BgfxRender::SetCullMode(ECullMode cullMode)
{
	m_renderState &= ~BGFX_STATE_CULL_MASK;

	switch(cullMode)
	{
		case ECullMode::None:	break;
		case ECullMode::CCW:	m_renderState |= BGFX_STATE_CULL_CCW; break;
		case ECullMode::CW:		m_renderState |= BGFX_STATE_CULL_CW; break;
	}
}

void BgfxRender::SetBlendModeSeparated(EBlend srcBlendRGB, EBlend destBlendRGB, EBlendOp blendOpRGB, EBlend srcBlendAlpha, EBlend destBlendAlpha, EBlendOp blendOpAlpha)
{
	m_renderState &= ~BGFX_STATE_BLEND_MASK;

	static constexpr uint64_t s_blendConversionTable[] =
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
	m_renderState |= BGFX_STATE_BLEND_FUNC_SEPARATE(srcRGB, destRGB, srcAlpha, destAlpha);

	static constexpr uint64_t s_equationConversionTable[] =
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
	m_renderState |= BGFX_STATE_BLEND_EQUATION_SEPARATE(equationRGB, equationAlpha);
}

void BgfxRender::SetBlendModeDisabled()
{
	m_renderState &= ~BGFX_STATE_BLEND_MASK;
}

void* BgfxRender::CreateRenderableRenderData(const Renderable& renderable)
{
	RenderableRenderDataBgfx* pRenderableRenderData = new RenderableRenderDataBgfx();

	const ShaderRenderDataBgfx* pVS = static_cast<const ShaderRenderDataBgfx*>(renderable.m_material->m_vertexShader->m_pRenderData);
	const ShaderRenderDataBgfx* pPS = static_cast<const ShaderRenderDataBgfx*>(renderable.m_material->m_pixelShader->m_pRenderData);
	pRenderableRenderData->m_shaderProgram = bgfx::createProgram(pVS->shaderHandle, pPS->shaderHandle);

	return pRenderableRenderData;
}

void* BgfxRender::CreateMeshRenderData(const Mesh& mesh)
{
	MeshRenderDataBgfx* pMeshRenderData = new MeshRenderDataBgfx();

	static constexpr bgfx::Attrib::Enum s_attribConversionTable[] =
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

	static constexpr bgfx::AttribType::Enum s_attribTypeConversionTable[] =
	{
		bgfx::AttribType::Enum::Float,	// Mesh::EAttribType::Float
		bgfx::AttribType::Enum::Uint8,	// Mesh::EAttribType::Uint8
	};
	DESIRE_CHECK_ARRAY_SIZE(s_attribTypeConversionTable, Mesh::EAttribType::Num);

	pMeshRenderData->vertexLayout.begin();
	const Array<Mesh::VertexLayout>& vertexLayout = mesh.GetVertexLayout();
	for(const Mesh::VertexLayout& layout : vertexLayout)
	{
		const bool isNormalized = (layout.m_type == Mesh::EAttribType::Uint8);
		pMeshRenderData->vertexLayout.add(s_attribConversionTable[(size_t)layout.m_attrib], layout.m_count, s_attribTypeConversionTable[(size_t)layout.m_type], isNormalized);
	}
	pMeshRenderData->vertexLayout.end();

	const bgfx::Memory* pIndexData = (mesh.GetNumIndices() != 0) ? bgfx::copy(mesh.m_indices.get(), mesh.GetSizeOfIndexData()) : nullptr;
	const bgfx::Memory* pVertexData = bgfx::copy(mesh.m_vertices.get(), mesh.GetSizeOfVertexData());

	switch(mesh.GetType())
	{
		case Mesh::EType::Static:
			if(pIndexData != nullptr)
			{
				pMeshRenderData->indexBuffer = bgfx::createIndexBuffer(pIndexData, (mesh.GetIndexSize() == sizeof(uint16_t)) ? BGFX_BUFFER_NONE : BGFX_BUFFER_INDEX32);
			}

			pMeshRenderData->vertexBuffer = bgfx::createVertexBuffer(pVertexData, pMeshRenderData->vertexLayout, BGFX_BUFFER_NONE);
			break;

		case Mesh::EType::Dynamic:
			if(pIndexData != nullptr)
			{
				pMeshRenderData->dynamicIndexBuffer = bgfx::createDynamicIndexBuffer(pIndexData, (mesh.GetIndexSize() == sizeof(uint16_t)) ? BGFX_BUFFER_NONE : BGFX_BUFFER_INDEX32);
			}

			pMeshRenderData->dynamicVertexBuffer = bgfx::createDynamicVertexBuffer(pVertexData, pMeshRenderData->vertexLayout, BGFX_BUFFER_NONE);
			break;
	}

	return pMeshRenderData;
}

void* BgfxRender::CreateShaderRenderData(const Shader& shader)
{
	ShaderRenderDataBgfx* pShaderRenderData = new ShaderRenderDataBgfx();

	const bgfx::Memory* shaderData = nullptr;
	if(shader.m_defines.empty())
	{
		shaderData = bgfx::makeRef(shader.m_data.ptr.get(), static_cast<uint32_t>(shader.m_data.size));
	}
	else
	{
		size_t totalDefinesLength = 0;
		for(const String& define : shader.m_defines)
		{
			totalDefinesLength += 8 + define.Length() + 3;	// "#define ASD 1\n"
		}

		shaderData = bgfx::alloc(static_cast<uint32_t>(totalDefinesLength + shader.m_data.size));
		uint8_t* ptr = shaderData->data;
		for(const String& define : shader.m_defines)
		{
			memcpy(ptr, "#define ", 8);
			ptr += 8;
			memcpy(ptr, define.Str(), define.Length());
			ptr += define.Length();
			memcpy(ptr, " 1\n", 3);
			ptr += 3;
		}
		memcpy(ptr, shader.m_data.ptr.get(), shader.m_data.size);
	}

	pShaderRenderData->shaderHandle = bgfx::createShader(shaderData);
	ASSERT(bgfx::isValid(pShaderRenderData->shaderHandle));
	bgfx::setName(pShaderRenderData->shaderHandle, shader.m_name.Str());

	const uint16_t uniformCount = bgfx::getShaderUniforms(pShaderRenderData->shaderHandle, nullptr, 0);
	bgfx::UniformHandle* uniforms = static_cast<bgfx::UniformHandle*>(alloca(uniformCount * sizeof(bgfx::UniformHandle)));
	bgfx::getShaderUniforms(pShaderRenderData->shaderHandle, uniforms, uniformCount);
	for(uint16_t i = 0; i < uniformCount; ++i)
	{
		bool isSampler = false;
		for(bgfx::UniformHandle& samplerUniform : m_samplerUniforms)
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
		pShaderRenderData->uniforms.Insert(HashedString::CreateFromString(String(info.name, strlen(info.name))), uniforms[i]);
	}

	return pShaderRenderData;
}

void* BgfxRender::CreateTextureRenderData(const Texture& texture)
{
	TextureRenderDataBgfx* pTextureRenderData = new TextureRenderDataBgfx();

	const bool isRenderTarget = (texture.GetData() == nullptr);

	if(isRenderTarget)
	{
		pTextureRenderData->textureHandle = bgfx::createTexture2D(texture.GetWidth(), texture.GetHeight(), (texture.GetNumMipLevels() > 1), 1, GetTextureFormat(texture), BGFX_TEXTURE_RT);
	}
	else
	{
		pTextureRenderData->textureHandle = bgfx::createTexture2D(texture.GetWidth(), texture.GetHeight(), (texture.GetNumMipLevels() > 1), 1, GetTextureFormat(texture), BGFX_TEXTURE_NONE, bgfx::makeRef(texture.GetData(), texture.GetDataSize()));
	}

	return pTextureRenderData;
}

void* BgfxRender::CreateRenderTargetRenderData(const RenderTarget& renderTarget)
{
	RenderTargetRenderDataBgfx* pRenderTargetRenderData = new RenderTargetRenderDataBgfx();

	bgfx::TextureHandle renderTargetTextures[BGFX_CONFIG_MAX_FRAME_BUFFER_ATTACHMENTS];
	const uint8_t textureCount = std::min<uint8_t>(renderTarget.GetTextureCount(), BGFX_CONFIG_MAX_FRAME_BUFFER_ATTACHMENTS);
	for(uint8_t i = 0; i < textureCount; ++i)
	{
		const std::shared_ptr<Texture>& texture = renderTarget.GetTexture(i);
		const TextureRenderDataBgfx* pTextureRenderData = static_cast<const TextureRenderDataBgfx*>(texture->m_pRenderData);
		ASSERT(pTextureRenderData != nullptr);

		renderTargetTextures[i] = pTextureRenderData->textureHandle;
	}

	pRenderTargetRenderData->frameBuffer = bgfx::createFrameBuffer(textureCount, renderTargetTextures);
	bgfx::setViewFrameBuffer(pRenderTargetRenderData->id, pRenderTargetRenderData->frameBuffer);

	return pRenderTargetRenderData;
}

void BgfxRender::DestroyRenderableRenderData(void* pRenderData)
{
	RenderableRenderDataBgfx* pRenderableRenderData = static_cast<RenderableRenderDataBgfx*>(pRenderData);

	bgfx::destroy(pRenderableRenderData->m_shaderProgram);

	delete pRenderableRenderData;
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

void BgfxRender::SetMesh(Mesh& mesh)
{
	DESIRE_UNUSED(mesh);
	// No-op
}

void BgfxRender::UpdateDynamicMesh(DynamicMesh& dynamicMesh)
{
	MeshRenderDataBgfx* pMeshRenderData = static_cast<MeshRenderDataBgfx*>(dynamicMesh.m_pRenderData);

	if(dynamicMesh.m_isIndicesDirty)
	{
		bgfx::update(pMeshRenderData->dynamicIndexBuffer, 0, bgfx::copy(dynamicMesh.m_indices.get(), dynamicMesh.GetSizeOfIndexData()));
		dynamicMesh.m_isIndicesDirty = false;
	}

	if(dynamicMesh.m_isVerticesDirty)
	{
		bgfx::update(pMeshRenderData->dynamicVertexBuffer, 0, bgfx::copy(dynamicMesh.m_vertices.get(), dynamicMesh.GetSizeOfVertexData()));
		dynamicMesh.m_isVerticesDirty = false;
	}
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

	const TextureRenderDataBgfx* pTextureRenderData = static_cast<const TextureRenderDataBgfx*>(texture.m_pRenderData);
	bgfx::setTexture(samplerIdx, m_samplerUniforms[samplerIdx], pTextureRenderData->textureHandle, flags);
}

void BgfxRender::SetRenderTarget(RenderTarget* pRenderTarget)
{
	if(pRenderTarget != nullptr)
	{
		RenderTargetRenderDataBgfx* pRenderTargetRenderData = static_cast<RenderTargetRenderDataBgfx*>(pRenderTarget->m_pRenderData);
		ASSERT(pRenderTargetRenderData->id < BGFX_CONFIG_MAX_VIEWS);
		m_activeViewId = pRenderTargetRenderData->id;
		bgfx::setViewRect(m_activeViewId, 0, 0, pRenderTarget->GetWidth(), pRenderTarget->GetHeight());
	}
	else
	{
		m_activeViewId = 0;
		bgfx::setViewRect(m_activeViewId, 0, 0, m_pActiveWindow->GetWidth(), m_pActiveWindow->GetHeight());
	}
}

void BgfxRender::UpdateShaderParams(const Material& material)
{
	const ShaderRenderDataBgfx* pVS = static_cast<const ShaderRenderDataBgfx*>(material.m_vertexShader->m_pRenderData);
	const ShaderRenderDataBgfx* pPS = static_cast<const ShaderRenderDataBgfx*>(material.m_pixelShader->m_pRenderData);

	for(const Material::ShaderParam& shaderParam : material.GetShaderParams())
	{
		const bgfx::UniformHandle* pUniform = pVS->uniforms.Find(shaderParam.m_name);
		if(pUniform == nullptr)
		{
			pUniform = pPS->uniforms.Find(shaderParam.m_name);
		}

		if(pUniform != nullptr && bgfx::isValid(*pUniform))
		{
			const void* value = shaderParam.GetValue();
			bgfx::setUniform(*pUniform, value);
		}
	}
}

void BgfxRender::DoRender(Renderable& renderable, uint32_t indexOffset, uint32_t vertexOffset, uint32_t numIndices, uint32_t numVertices)
{
	RenderableRenderDataBgfx* pRenderableRenderData = static_cast<RenderableRenderDataBgfx*>(renderable.m_pRenderData);

	MeshRenderDataBgfx* pMeshRenderData = static_cast<MeshRenderDataBgfx*>(m_pActiveMesh->m_pRenderData);
	switch(m_pActiveMesh->GetType())
	{
		case Mesh::EType::Static:
		{
			bgfx::setIndexBuffer(pMeshRenderData->indexBuffer, indexOffset, numIndices);
			bgfx::setVertexBuffer(0, pMeshRenderData->vertexBuffer, vertexOffset, numVertices);
			break;
		}

		case Mesh::EType::Dynamic:
		{
			bgfx::setIndexBuffer(pMeshRenderData->dynamicIndexBuffer, indexOffset, numIndices);
			bgfx::setVertexBuffer(0, pMeshRenderData->dynamicVertexBuffer, vertexOffset, numVertices);
			break;
		}
	}

	bgfx::setState(m_renderState, m_blendFactor);

	bgfx::submit(m_activeViewId, pRenderableRenderData->m_shaderProgram, 0, BGFX_DISCARD_NONE);
}

bgfx::TextureFormat::Enum BgfxRender::GetTextureFormat(const Texture& texture)
{
	static constexpr bgfx::TextureFormat::Enum conversionTable[] =
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

	return conversionTable[static_cast<size_t>(texture.GetFormat())];
}
