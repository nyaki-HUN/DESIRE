#include "stdafx_bgfx.h"
#include "BgfxRender.h"

#include "RenderData/IndexBufferRenderDataBgfx.h"
#include "RenderData/RenderableRenderDataBgfx.h"
#include "RenderData/RenderTargetRenderDataBgfx.h"
#include "RenderData/ShaderRenderDataBgfx.h"
#include "RenderData/TextureRenderDataBgfx.h"
#include "RenderData/VertexBufferRenderDataBgfx.h"
#include "ToBgfx.h"

#include "Engine/Application/OSWindow.h"

#include "Engine/Core/Math/Matrix4.h"
#include "Engine/Core/String/StackString.h"

#include "Engine/Render/IndexBuffer.h"
#include "Engine/Render/Material.h"
#include "Engine/Render/Renderable.h"
#include "Engine/Render/RenderTarget.h"
#include "Engine/Render/Shader.h"
#include "Engine/Render/Texture.h"
#include "Engine/Render/VertexBuffer.h"

#include "bgfx/../../src/config.h"

static_assert(Render::kMaxTextureSamplers == BGFX_CONFIG_MAX_TEXTURE_SAMPLERS);

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
	m_pActiveIndexBuffer = nullptr;
	m_pActiveVertexBuffer = nullptr;
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

void BgfxRender::SetScissor(uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
	bgfx::setScissor(x, y, width, height);
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

RenderData* BgfxRender::CreateRenderableRenderData(const Renderable& renderable)
{
	RenderableRenderDataBgfx* pRenderableRenderData = new RenderableRenderDataBgfx();

	const ShaderRenderDataBgfx* pVS = static_cast<const ShaderRenderDataBgfx*>(renderable.m_spMaterial->m_spVertexShader->m_pRenderData);
	const ShaderRenderDataBgfx* pPS = static_cast<const ShaderRenderDataBgfx*>(renderable.m_spMaterial->m_spPixelShader->m_pRenderData);
	pRenderableRenderData->m_shaderProgram = bgfx::createProgram(pVS->m_shaderHandle, pPS->m_shaderHandle);

	pRenderableRenderData->m_renderState = BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_MSAA;

	switch(renderable.m_spMaterial->m_cullMode)
	{
		case ECullMode::None:	break;
		case ECullMode::CCW:	pRenderableRenderData->m_renderState |= BGFX_STATE_CULL_CCW; break;
		case ECullMode::CW:		pRenderableRenderData->m_renderState |= BGFX_STATE_CULL_CW; break;
	}

	if(renderable.m_spMaterial->m_isBlendEnabled)
	{
		pRenderableRenderData->m_renderState |= BGFX_STATE_BLEND_FUNC_SEPARATE(ToBgfx(renderable.m_spMaterial->m_srcBlendRGB), ToBgfx(renderable.m_spMaterial->m_destBlendRGB), ToBgfx(renderable.m_spMaterial->m_srcBlendAlpha), ToBgfx(renderable.m_spMaterial->m_destBlendAlpha));
		pRenderableRenderData->m_renderState |= BGFX_STATE_BLEND_EQUATION_SEPARATE(ToBgfx(renderable.m_spMaterial->m_blendOpRGB), ToBgfx(renderable.m_spMaterial->m_blendOpAlpha));
	}

	static_assert(static_cast<uint8_t>(EColorWrite::Red) == BGFX_STATE_WRITE_R);
	static_assert(static_cast<uint8_t>(EColorWrite::Green) == BGFX_STATE_WRITE_G);
	static_assert(static_cast<uint8_t>(EColorWrite::Blue) == BGFX_STATE_WRITE_B);
	static_assert(static_cast<uint8_t>(EColorWrite::Alpha) == BGFX_STATE_WRITE_A);

	pRenderableRenderData->m_renderState |= static_cast<uint8_t>(renderable.m_spMaterial->m_colorWriteMask);

	switch(renderable.m_spMaterial->m_depthTest)
	{
		case EDepthTest::Disabled:		break;
		case EDepthTest::Less:			pRenderableRenderData->m_renderState |= BGFX_STATE_DEPTH_TEST_LESS; break;
		case EDepthTest::LessEqual:		pRenderableRenderData->m_renderState |= BGFX_STATE_DEPTH_TEST_LEQUAL; break;
		case EDepthTest::Greater:		pRenderableRenderData->m_renderState |= BGFX_STATE_DEPTH_TEST_GREATER; break;
		case EDepthTest::GreaterEqual:	pRenderableRenderData->m_renderState |= BGFX_STATE_DEPTH_TEST_GEQUAL; break;
		case EDepthTest::Equal:			pRenderableRenderData->m_renderState |= BGFX_STATE_DEPTH_TEST_EQUAL; break;
		case EDepthTest::NotEqual:		pRenderableRenderData->m_renderState |= BGFX_STATE_DEPTH_TEST_NOTEQUAL; break;
	}

	if(renderable.m_spMaterial->m_isDepthWriteEnabled)
	{
		pRenderableRenderData->m_renderState |= BGFX_STATE_WRITE_Z;
	}

	return pRenderableRenderData;
}

RenderData* BgfxRender::CreateIndexBufferRenderData(const IndexBuffer& indexBuffer)
{
	IndexBufferRenderDataBgfx* pIndexBufferRenderData = new IndexBufferRenderDataBgfx();

	const bgfx::Memory* pData = bgfx::copy(indexBuffer.GetData(), indexBuffer.GetDataSize());
	uint16_t flags = BGFX_BUFFER_NONE;
	if(indexBuffer.GetIndexSize() == sizeof(uint32_t))
	{
		flags |= BGFX_BUFFER_INDEX32;
	}

	if(indexBuffer.GetFlags() & DeviceBuffer::EFlags::STATIC)
	{
		pIndexBufferRenderData->m_indexBuffer = bgfx::createIndexBuffer(pData, flags);
	}
	else
	{
		pIndexBufferRenderData->m_dynamicIndexBuffer = bgfx::createDynamicIndexBuffer(pData, flags);
	}

	return pIndexBufferRenderData;
}

RenderData* BgfxRender::CreateVertexBufferRenderData(const VertexBuffer& vertexBuffer)
{
	VertexBufferRenderDataBgfx* pVertexBufferRenderData = new VertexBufferRenderDataBgfx();

	pVertexBufferRenderData->m_vertexLayout.begin();
	const Array<VertexBuffer::Layout>& vertexLayout = vertexBuffer.GetVertexLayout();
	for(const VertexBuffer::Layout& layout : vertexLayout)
	{
		const bool isNormalized = (layout.type == VertexBuffer::EAttribType::Uint8);
		pVertexBufferRenderData->m_vertexLayout.add(ToBgfx(layout.attrib), layout.count, ToBgfx(layout.type), isNormalized);
	}
	pVertexBufferRenderData->m_vertexLayout.end();

	const bgfx::Memory* pData = bgfx::copy(vertexBuffer.GetData(), vertexBuffer.GetDataSize());
	uint16_t flags = BGFX_BUFFER_NONE;

	if(vertexBuffer.GetFlags() & DeviceBuffer::STATIC)
	{
		pVertexBufferRenderData->m_vertexBuffer = bgfx::createVertexBuffer(pData, pVertexBufferRenderData->m_vertexLayout, flags);
	}
	else
	{
		pVertexBufferRenderData->m_dynamicVertexBuffer = bgfx::createDynamicVertexBuffer(pData, pVertexBufferRenderData->m_vertexLayout, flags);
	}

	return pVertexBufferRenderData;
}

RenderData* BgfxRender::CreateShaderRenderData(const Shader& shader)
{
	ShaderRenderDataBgfx* pShaderRenderData = new ShaderRenderDataBgfx();

	const bgfx::Memory* pShaderData = nullptr;
	if(shader.m_defines.empty())
	{
		pShaderData = bgfx::makeRef(shader.m_data.GetData(), static_cast<uint32_t>(shader.m_data.GetSize()));
	}
	else
	{
		size_t totalDefinesLength = 0;
		for(const String& define : shader.m_defines)
		{
			totalDefinesLength += 8 + define.Length() + 3;	// "#define ASD 1\n"
		}

		pShaderData = bgfx::alloc(static_cast<uint32_t>(totalDefinesLength + shader.m_data.GetSize()));
		uint8_t* ptr = pShaderData->data;
		for(const String& define : shader.m_defines)
		{
			memcpy(ptr, "#define ", 8);
			ptr += 8;
			memcpy(ptr, define.Str(), define.Length());
			ptr += define.Length();
			memcpy(ptr, " 1\n", 3);
			ptr += 3;
		}
		memcpy(ptr, shader.m_data.GetData(), shader.m_data.GetSize());
	}

	pShaderRenderData->m_shaderHandle = bgfx::createShader(pShaderData);
	ASSERT(bgfx::isValid(pShaderRenderData->m_shaderHandle));
	bgfx::setName(pShaderRenderData->m_shaderHandle, shader.m_name.Str());

	const uint16_t uniformCount = bgfx::getShaderUniforms(pShaderRenderData->m_shaderHandle);
	DESIRE_STACKALLOCATE_ARRAY(bgfx::UniformHandle, uniforms, uniformCount);
	bgfx::getShaderUniforms(pShaderRenderData->m_shaderHandle, uniforms, uniformCount);
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
		pShaderRenderData->m_uniforms.Insert(String(info.name, strlen(info.name)), uniforms[i]);
	}

	return pShaderRenderData;
}

RenderData* BgfxRender::CreateTextureRenderData(const Texture& texture)
{
	TextureRenderDataBgfx* pTextureRenderData = new TextureRenderDataBgfx();

	const bool isRenderTarget = (texture.GetData() == nullptr);

	if(isRenderTarget)
	{
		pTextureRenderData->m_textureHandle = bgfx::createTexture2D(texture.GetWidth(), texture.GetHeight(), (texture.GetNumMipLevels() > 1), 1, ToBgfx(texture.GetFormat()), BGFX_TEXTURE_RT);
	}
	else
	{
		pTextureRenderData->m_textureHandle = bgfx::createTexture2D(texture.GetWidth(), texture.GetHeight(), (texture.GetNumMipLevels() > 1), 1, ToBgfx(texture.GetFormat()), BGFX_TEXTURE_NONE, bgfx::makeRef(texture.GetData(), texture.GetDataSize()));
	}

	return pTextureRenderData;
}

RenderData* BgfxRender::CreateRenderTargetRenderData(const RenderTarget& renderTarget)
{
	RenderTargetRenderDataBgfx* pRenderTargetRenderData = new RenderTargetRenderDataBgfx();

	bgfx::TextureHandle renderTargetTextures[BGFX_CONFIG_MAX_FRAME_BUFFER_ATTACHMENTS];
	const uint8_t textureCount = std::min<uint8_t>(renderTarget.GetTextureCount(), BGFX_CONFIG_MAX_FRAME_BUFFER_ATTACHMENTS);
	for(uint8_t i = 0; i < textureCount; ++i)
	{
		const std::shared_ptr<Texture>& texture = renderTarget.GetTexture(i);
		const TextureRenderDataBgfx* pTextureRenderData = static_cast<const TextureRenderDataBgfx*>(texture->m_pRenderData);
		ASSERT(pTextureRenderData != nullptr);

		renderTargetTextures[i] = pTextureRenderData->m_textureHandle;
	}

	pRenderTargetRenderData->m_frameBuffer = bgfx::createFrameBuffer(textureCount, renderTargetTextures);
	bgfx::setViewFrameBuffer(pRenderTargetRenderData->m_id, pRenderTargetRenderData->m_frameBuffer);

	return pRenderTargetRenderData;
}

void BgfxRender::SetIndexBuffer(IndexBuffer& indexBuffer)
{
	DESIRE_UNUSED(indexBuffer);
	// No-op
}

void BgfxRender::SetVertexBuffer(VertexBuffer& vertexBuffer)
{
	DESIRE_UNUSED(vertexBuffer);
	// No-op
}

void BgfxRender::SetRenderTarget(RenderTarget* pRenderTarget)
{
	if(pRenderTarget)
	{
		RenderTargetRenderDataBgfx* pRenderTargetRenderData = static_cast<RenderTargetRenderDataBgfx*>(pRenderTarget->m_pRenderData);
		ASSERT(pRenderTargetRenderData->m_id < BGFX_CONFIG_MAX_VIEWS);
		m_activeViewId = pRenderTargetRenderData->m_id;
		bgfx::setViewRect(m_activeViewId, 0, 0, pRenderTarget->GetWidth(), pRenderTarget->GetHeight());
	}
	else
	{
		m_activeViewId = 0;
		bgfx::setViewRect(m_activeViewId, 0, 0, m_pActiveWindow->GetWidth(), m_pActiveWindow->GetHeight());
	}
}

void BgfxRender::UpdateDeviceBuffer(DeviceBuffer& deviceBuffer)
{
/*	DeviceBufferRenderDataBgfx* pDeviceBufferRenderData = static_cast<DeviceBufferRenderDataBgfx*>(deviceBuffer.m_pRenderData);

	bgfx::update(pMeshRenderData->m_dynamicIndexBuffer, 0, bgfx::copy(dynamicMesh.m_spIndices.get(), dynamicMesh.GetSizeOfIndexData()));
	dynamicMesh.m_isIndicesDirty = false;
	}

	if(dynamicMesh.m_isVerticesDirty)
	{
		bgfx::update(pMeshRenderData->m_dynamicVertexBuffer, 0, bgfx::copy(dynamicMesh.m_spVertices.get(), dynamicMesh.GetSizeOfVertexData()));
		dynamicMesh.m_isVerticesDirty = false;
	}
*/
}

void BgfxRender::UpdateShaderParams(const Material& material)
{
	const ShaderRenderDataBgfx* pVS = static_cast<const ShaderRenderDataBgfx*>(material.m_spVertexShader->m_pRenderData);
	const ShaderRenderDataBgfx* pPS = static_cast<const ShaderRenderDataBgfx*>(material.m_spPixelShader->m_pRenderData);

	for(const Material::ShaderParam& shaderParam : material.GetShaderParams())
	{
		const bgfx::UniformHandle* pUniform = pVS->m_uniforms.Find(shaderParam.name);
		if(pUniform == nullptr)
		{
			pUniform = pPS->m_uniforms.Find(shaderParam.name);
		}

		if(pUniform && bgfx::isValid(*pUniform))
		{
			const void* pValue = shaderParam.GetValue();
			bgfx::setUniform(*pUniform, pValue);
		}
	}

	ASSERT(material.GetTextures().Size() <= Render::kMaxTextureSamplers);
	uint8_t samplerIdx = 0;
	for(const Material::TextureInfo& textureInfo : material.GetTextures())
	{
		const TextureRenderDataBgfx* pTextureRenderData = static_cast<const TextureRenderDataBgfx*>(textureInfo.spTexture->m_pRenderData);

		uint32_t flags = BGFX_TEXTURE_NONE;
		switch(textureInfo.filterMode)
		{
			case EFilterMode::Point:		flags |= BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT | BGFX_SAMPLER_MIP_POINT; break;
			case EFilterMode::Bilinear:		flags |= BGFX_SAMPLER_MIP_POINT; break;
			case EFilterMode::Trilinear:	break;
			case EFilterMode::Anisotropic:	flags |= BGFX_SAMPLER_MIN_ANISOTROPIC | BGFX_SAMPLER_MAG_ANISOTROPIC; break;
		}

		switch(textureInfo.addressMode)
		{
			case EAddressMode::Repeat:			break;
			case EAddressMode::Clamp:			flags |= BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_W_CLAMP; break;
			case EAddressMode::MirroredRepeat:	flags |= BGFX_SAMPLER_U_MIRROR | BGFX_SAMPLER_V_MIRROR | BGFX_SAMPLER_W_MIRROR; break;
			case EAddressMode::MirrorOnce:		flags |= BGFX_SAMPLER_U_MIRROR | BGFX_SAMPLER_V_MIRROR | BGFX_SAMPLER_W_MIRROR; break;
			case EAddressMode::Border:			flags |= BGFX_SAMPLER_U_BORDER | BGFX_SAMPLER_V_BORDER | BGFX_SAMPLER_W_BORDER; break;
		}

		bgfx::setTexture(samplerIdx, m_samplerUniforms[samplerIdx], pTextureRenderData->m_textureHandle, flags);
		samplerIdx++;
	}
}

void BgfxRender::DoRender(Renderable& renderable, uint32_t indexOffset, uint32_t vertexOffset, uint32_t numIndices, uint32_t numVertices)
{
	RenderableRenderDataBgfx* pRenderableRenderData = static_cast<RenderableRenderDataBgfx*>(renderable.m_pRenderData);
	bgfx::setState(pRenderableRenderData->m_renderState, m_blendFactor);

	if(m_pActiveIndexBuffer)
	{
		IndexBufferRenderDataBgfx* pIndexBufferRenderData = static_cast<IndexBufferRenderDataBgfx*>(m_pActiveIndexBuffer->m_pRenderData);
		if(bgfx::isValid(pIndexBufferRenderData->m_indexBuffer))
		{
			bgfx::setIndexBuffer(pIndexBufferRenderData->m_indexBuffer, indexOffset, numIndices);
		}
		else if(bgfx::isValid(pIndexBufferRenderData->m_dynamicIndexBuffer))
		{
			bgfx::setIndexBuffer(pIndexBufferRenderData->m_dynamicIndexBuffer, indexOffset, numIndices);
		}
		else
		{
			ASSERT(false);
		}
	}

	VertexBufferRenderDataBgfx* pVertexBufferRenderData = static_cast<VertexBufferRenderDataBgfx*>(m_pActiveVertexBuffer->m_pRenderData);
	if(bgfx::isValid(pVertexBufferRenderData->m_vertexBuffer))
	{
		bgfx::setVertexBuffer(0, pVertexBufferRenderData->m_vertexBuffer, vertexOffset, numVertices);
	}
	else if(bgfx::isValid(pVertexBufferRenderData->m_dynamicVertexBuffer))
	{
		bgfx::setVertexBuffer(0, pVertexBufferRenderData->m_dynamicVertexBuffer, vertexOffset, numVertices);
	}
	else
	{
		ASSERT(false);
	}

	bgfx::submit(m_activeViewId, pRenderableRenderData->m_shaderProgram, 0, BGFX_DISCARD_NONE);
}
