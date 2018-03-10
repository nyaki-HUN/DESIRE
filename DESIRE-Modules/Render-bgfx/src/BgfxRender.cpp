#include "BgfxRender.h"
#include "MeshRenderDataBgfx.h"
#include "ShaderRenderDataBgfx.h"
#include "RenderTargetRenderDataBgfx.h"
#include "TextureRenderDataBgfx.h"
#include "Embedded_shaders/vs_screenSpaceQuad.bin.h"

#include "Engine/Core/assert.h"
#include "Engine/Core/IWindow.h"
#include "Engine/Core/String.h"
#include "Engine/Core/math/Matrix4.h"
#include "Engine/Render/Material.h"
#include "Engine/Render/RenderTarget.h"
#include "Engine/Render/View.h"
#include "Engine/Resource/Mesh.h"
#include "Engine/Resource/Shader.h"

#include "bgfx/platform.h"
#include "bgfx/embedded_shader.h"
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

	screenSpaceQuadMeshVertexDecl.begin();
	screenSpaceQuadMeshVertexDecl.add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float);
	screenSpaceQuadMeshVertexDecl.end();
}

BgfxRender::~BgfxRender()
{

}

void BgfxRender::Init(IWindow *mainWindow)
{
	bgfx::PlatformData pd = {};
#if defined(DESIRE_PLATFORM_LINUX)
	pd.ndt = GetDisplay();
#endif
	pd.nwh = mainWindow->GetHandle();
	bgfx::setPlatformData(pd);

	initialized = bgfx::init(bgfx::RendererType::Count, BGFX_PCI_ID_NONE);
	activeViewId = 0;

	for(bgfx::ViewId viewId = 0; viewId < BGFX_CONFIG_MAX_VIEWS; ++viewId)
	{
		bgfx::setViewClear(viewId, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH | BGFX_CLEAR_STENCIL, clearColor, 1.0f, 0);
	}

	samplerUniforms[0] = bgfx::createUniform("s_tex", bgfx::UniformType::Int1);
	for(uint8_t i = 1; i < DESIRE_ASIZEOF(samplerUniforms); ++i)
	{
		samplerUniforms[i] = bgfx::createUniform(String::CreateFormattedString("s_tex%u", i).c_str(), bgfx::UniformType::Int1);
	}

	renderState = BGFX_STATE_MSAA;
	SetDefaultRenderStates();

	BindEmbeddedShader(screenSpaceQuadVertexShader.get(), "vs_screenSpaceQuad");

	bgfx::reset(mainWindow->GetWidth(), mainWindow->GetHeight(), BGFX_RESET_VSYNC);
}

void BgfxRender::UpdateRenderWindow(IWindow *window)
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

	activeVertexShader = nullptr;
	activeFragmentShader = nullptr;

	Unbind(screenSpaceQuadVertexShader.get());

	bgfx::shutdown();
	initialized = false;
}

String BgfxRender::GetShaderFilenameWithPath(const char *shaderFilename) const
{
	const char *shaderLanguage = "";
	switch(bgfx::getRendererType())
	{
		case bgfx::RendererType::Direct3D9:		shaderLanguage = "dx9"; break;
		case bgfx::RendererType::Direct3D11:	shaderLanguage = "dx11"; break;
		case bgfx::RendererType::Direct3D12:	shaderLanguage = "dx11"; break;
		case bgfx::RendererType::Gnm:			shaderLanguage = "pssl"; break;
		case bgfx::RendererType::Metal:			shaderLanguage = "metal"; break;
		case bgfx::RendererType::OpenGLES:		shaderLanguage = "essl"; break;
		case bgfx::RendererType::OpenGL:		shaderLanguage = "glsl"; break;
		case bgfx::RendererType::Vulkan:		shaderLanguage = "spirv"; break;
		default:								ASSERT(false && "Not supported renderer type"); break;
	}

	return String::CreateFormattedString("data/shaders/bgfx/%s/%s.bin", shaderLanguage, shaderFilename);
}

void BgfxRender::BeginFrame(IWindow *window)
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

void BgfxRender::SetView(View *view)
{
	if(view != nullptr)
	{
		RenderTarget *rt = view->GetRenderTarget();
		if(rt->renderData == nullptr)
		{
			Bind(rt);
		}

		RenderTargetRenderDataBgfx *renderData = static_cast<RenderTargetRenderDataBgfx*>(rt->renderData);
		ASSERT(renderData->id < BGFX_CONFIG_MAX_VIEWS);
		activeViewId = renderData->id;
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

void BgfxRender::SetColorWriteEnabled(bool rgbWriteEnabled, bool alphaWriteEnabled)
{
	if(rgbWriteEnabled)
	{
		renderState |= BGFX_STATE_RGB_WRITE;
	}
	else
	{
		renderState &= ~BGFX_STATE_RGB_WRITE;
	}

	if(alphaWriteEnabled)
	{
		renderState |= BGFX_STATE_ALPHA_WRITE;
	}
	else
	{
		renderState &= ~BGFX_STATE_ALPHA_WRITE;
	}
}

void BgfxRender::SetDepthWriteEnabled(bool enabled)
{
	if(enabled)
	{
		renderState |= BGFX_STATE_DEPTH_WRITE;
	}
	else
	{
		renderState &= ~BGFX_STATE_DEPTH_WRITE;
	}
}

void BgfxRender::SetDepthTest(EDepthTest depthTest)
{
	renderState &= ~BGFX_STATE_DEPTH_TEST_MASK;

	switch(depthTest)
	{
		case EDepthTest::DISABLED:		break;
		case EDepthTest::LESS:			renderState |= BGFX_STATE_DEPTH_TEST_LESS; break;
		case EDepthTest::LESS_EQUAL:	renderState |= BGFX_STATE_DEPTH_TEST_LEQUAL; break;
		case EDepthTest::GREATER:		renderState |= BGFX_STATE_DEPTH_TEST_GREATER; break;
		case EDepthTest::GREATER_EQUAL:	renderState |= BGFX_STATE_DEPTH_TEST_GEQUAL; break;
		case EDepthTest::EQUAL:			renderState |= BGFX_STATE_DEPTH_TEST_EQUAL; break;
		case EDepthTest::NOT_EQUAL:		renderState |= BGFX_STATE_DEPTH_TEST_NOTEQUAL; break;
	}
}

void BgfxRender::SetCullMode(ECullMode cullMode)
{
	renderState &= ~BGFX_STATE_CULL_MASK;

	switch(cullMode)
	{
		case ECullMode::NONE:	break;
		case ECullMode::CCW:	renderState |= BGFX_STATE_CULL_CCW; break;
		case ECullMode::CW:		renderState |= BGFX_STATE_CULL_CW; break;
	}
}

void BgfxRender::SetBlendModeSeparated(EBlend srcBlendRGB, EBlend destBlendRGB, EBlendOp blendOpRGB, EBlend srcBlendAlpha, EBlend destBlendAlpha, EBlendOp blendOpAlpha)
{
	renderState &= ~BGFX_STATE_BLEND_MASK;

	static const uint64_t blendConversionTable[] =
	{
		BGFX_STATE_BLEND_ZERO,				// EBlend::ZERO
		BGFX_STATE_BLEND_ONE,				// EBlend::ONE
		BGFX_STATE_BLEND_SRC_COLOR,			// EBlend::SRC_COLOR
		BGFX_STATE_BLEND_INV_SRC_COLOR,		// EBlend::INV_SRC_COLOR
		BGFX_STATE_BLEND_SRC_ALPHA,			// EBlend::SRC_ALPHA
		BGFX_STATE_BLEND_INV_SRC_ALPHA,		// EBlend::INV_SRC_ALPHA
		BGFX_STATE_BLEND_DST_ALPHA,			// EBlend::DEST_ALPHA
		BGFX_STATE_BLEND_INV_DST_ALPHA,		// EBlend::INV_DEST_ALPHA
		BGFX_STATE_BLEND_DST_COLOR,			// EBlend::DEST_COLOR
		BGFX_STATE_BLEND_INV_DST_COLOR,		// EBlend::INV_DEST_COLOR
		BGFX_STATE_BLEND_SRC_ALPHA_SAT,		// EBlend::SRC_ALPHA_SAT
		BGFX_STATE_BLEND_FACTOR,			// EBlend::BLEND_FACTOR
		BGFX_STATE_BLEND_INV_FACTOR			// EBlend::INV_BLEND_FACTOR
	};
	DESIRE_CHECK_ARRAY_SIZE(blendConversionTable, EBlend::INV_BLEND_FACTOR + 1);

	const uint64_t srcRGB = blendConversionTable[(size_t)srcBlendRGB];
	const uint64_t destRGB = blendConversionTable[(size_t)destBlendRGB];
	const uint64_t srcAlpha = blendConversionTable[(size_t)srcBlendAlpha];
	const uint64_t destAlpha = blendConversionTable[(size_t)destBlendAlpha];
	renderState |= BGFX_STATE_BLEND_FUNC_SEPARATE(srcRGB, destRGB, srcAlpha, destAlpha);

	static const uint64_t equationConversionTable[] =
	{
		BGFX_STATE_BLEND_EQUATION_ADD,		// EBlendOp::ADD
		BGFX_STATE_BLEND_EQUATION_SUB,		// EBlendOp::SUBTRACT
		BGFX_STATE_BLEND_EQUATION_REVSUB,	// EBlendOp::REV_SUBTRACT
		BGFX_STATE_BLEND_EQUATION_MIN,		// EBlendOp::MIN
		BGFX_STATE_BLEND_EQUATION_MAX		// EBlendOp::MAX
	};
	DESIRE_CHECK_ARRAY_SIZE(equationConversionTable, EBlendOp::MAX + 1);

	const uint64_t equationRGB = equationConversionTable[(size_t)blendOpRGB];
	const uint64_t equationAlpha = equationConversionTable[(size_t)blendOpAlpha];
	renderState |= BGFX_STATE_BLEND_EQUATION_SEPARATE(equationRGB, equationAlpha);
}

void BgfxRender::SetBlendModeDisabled()
{
	renderState &= ~BGFX_STATE_BLEND_MASK;
}

void BgfxRender::Bind(Mesh *mesh)
{
	ASSERT(mesh != nullptr);

	if(mesh->renderData != nullptr)
	{
		// Already bound
		return;
	}

	MeshRenderDataBgfx *renderData = new MeshRenderDataBgfx();

	static const bgfx::Attrib::Enum attribConversionTable[] =
	{
		bgfx::Attrib::Position,		// Mesh::EAttrib::POSITION
		bgfx::Attrib::Normal,		// Mesh::EAttrib::NORMAL
		bgfx::Attrib::Color0,		// Mesh::EAttrib::COLOR
		bgfx::Attrib::TexCoord0,	// Mesh::EAttrib::TEXCOORD0
		bgfx::Attrib::TexCoord1,	// Mesh::EAttrib::TEXCOORD1
		bgfx::Attrib::TexCoord2,	// Mesh::EAttrib::TEXCOORD2
		bgfx::Attrib::TexCoord3,	// Mesh::EAttrib::TEXCOORD3
		bgfx::Attrib::TexCoord4,	// Mesh::EAttrib::TEXCOORD4
		bgfx::Attrib::TexCoord5,	// Mesh::EAttrib::TEXCOORD5
		bgfx::Attrib::TexCoord6,	// Mesh::EAttrib::TEXCOORD6
		bgfx::Attrib::TexCoord7,	// Mesh::EAttrib::TEXCOORD7
	};
	DESIRE_CHECK_ARRAY_SIZE(attribConversionTable, Mesh::EAttrib::NUM);

	static const bgfx::AttribType::Enum attribTypeConversionTable[] =
	{
		bgfx::AttribType::Enum::Float,	// Mesh::EAttribType::FLOAT
		bgfx::AttribType::Enum::Uint8,	// Mesh::EAttribType::UINT8
	};
	DESIRE_CHECK_ARRAY_SIZE(attribTypeConversionTable, Mesh::EAttribType::NUM);

	renderData->vertexDecl.begin();
	for(Mesh::VertexDecl& decl : mesh->vertexDecl)
	{
		const bool isNormalized = (decl.type == Mesh::EAttribType::UINT8);
		renderData->vertexDecl.add(attribConversionTable[(size_t)decl.attrib], decl.count, attribTypeConversionTable[(size_t)decl.type], isNormalized);
	}
	renderData->vertexDecl.end();

	switch(mesh->type)
	{
		case Mesh::EType::STATIC:
		{
			if(mesh->numIndices != 0)
			{
				const bgfx::Memory *indexData = bgfx::makeRef(mesh->indices.get(), mesh->GetSizeOfIndices());
				renderData->indexBuffer = bgfx::createIndexBuffer(indexData, BGFX_BUFFER_NONE);
			}

			if(mesh->numVertices != 0)
			{
				const bgfx::Memory *vertexData = bgfx::makeRef(mesh->vertices.get(), mesh->GetSizeOfVertices());
				renderData->vertexBuffer = bgfx::createVertexBuffer(vertexData, renderData->vertexDecl, BGFX_BUFFER_NONE);
			}
			break;
		}

		case Mesh::EType::DYNAMIC:
		{
			DynamicMesh *dynamicMesh = static_cast<DynamicMesh*>(mesh);

			if(dynamicMesh->maxNumOfIndices != 0)
			{
				const bgfx::Memory *indexData = bgfx::copy(dynamicMesh->indices.get(), dynamicMesh->GetMaxSizeOfIndices());
				renderData->dynamicIndexBuffer = bgfx::createDynamicIndexBuffer(indexData, BGFX_BUFFER_NONE);
			}

			if(dynamicMesh->maxNumOfVertices != 0)
			{
				const bgfx::Memory *vertexData = bgfx::copy(dynamicMesh->vertices.get(), dynamicMesh->GetMaxSizeOfVertices());
				renderData->dynamicVertexBuffer = bgfx::createDynamicVertexBuffer(vertexData, renderData->vertexDecl, BGFX_BUFFER_NONE);
			}
			break;
		}
	}

	mesh->renderData = renderData;
}

void BgfxRender::Bind(Shader *shader)
{
	ASSERT(shader != nullptr);

	if(shader->renderData != nullptr)
	{
		// Already bound
		return;
	}
	
	ShaderRenderDataBgfx *renderData = new ShaderRenderDataBgfx();

	const bgfx::Memory *shaderData = nullptr;
	if(shader->defines.empty())
	{
		shaderData = bgfx::makeRef(shader->data.data, (uint32_t)shader->data.size);
	}
	else
	{
		size_t totalDefinesLength = 0;
		for(const String& define : shader->defines)
		{
			totalDefinesLength += 8 + define.Length() + 3;	// "#define ASD 1\n"
		}

		shaderData = bgfx::alloc((uint32_t)(totalDefinesLength + shader->data.size));
		uint8_t *ptr = shaderData->data;
		for(const String& define : shader->defines)
		{
			memcpy(ptr, "#define ", 8);
			ptr += 8;
			memcpy(ptr, define.c_str(), define.Length());
			ptr += define.Length();
			memcpy(ptr, " 1\n", 3);
			ptr += 3;
		}
		memcpy(ptr, shader->data.data, shader->data.size);
	}

	renderData->shaderHandle = bgfx::createShader(shaderData);
	ASSERT(bgfx::isValid(renderData->shaderHandle));
	bgfx::setName(renderData->shaderHandle, shader->name.c_str());

	const uint16_t uniformCount = bgfx::getShaderUniforms(renderData->shaderHandle, nullptr, 0);
	bgfx::UniformHandle *uniforms = (bgfx::UniformHandle*)alloca(uniformCount * sizeof(bgfx::UniformHandle));
	bgfx::getShaderUniforms(renderData->shaderHandle, uniforms, uniformCount);
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
		renderData->uniforms.Insert(HashedString::CreateFromDynamicString(info.name), uniforms[i]);
	}

	shader->renderData = renderData;
}

void BgfxRender::Bind(Texture *texture)
{
	ASSERT(texture != nullptr);

	if(texture->renderData != nullptr)
	{
		// Already bound
		return;
	}

	TextureRenderDataBgfx *renderData = new TextureRenderDataBgfx();

	const bool isRenderTarget = (texture->data.data == nullptr);

	if(isRenderTarget)
	{
		renderData->textureHandle = bgfx::createTexture2D(texture->width, texture->height, (texture->numMipMaps != 0), 1, ConvertTextureFormat(texture->format), BGFX_TEXTURE_RT);
	}
	else
	{
		renderData->textureHandle = bgfx::createTexture2D(texture->width, texture->height, (texture->numMipMaps != 0), 1, ConvertTextureFormat(texture->format), BGFX_TEXTURE_NONE, bgfx::makeRef(texture->data.data, (uint32_t)texture->data.size));
	}

	texture->renderData = renderData;
}

void BgfxRender::Bind(RenderTarget *renderTarget)
{
	ASSERT(renderTarget != nullptr);

	if(renderTarget->renderData != nullptr)
	{
		// Already bound
		return;
	}

	RenderTargetRenderDataBgfx *renderData = new RenderTargetRenderDataBgfx();

	bgfx::TextureHandle renderTargetTextures[BGFX_CONFIG_MAX_FRAME_BUFFER_ATTACHMENTS];
	const uint8_t textureCount = std::min<uint8_t>(renderTarget->GetTextureCount(), BGFX_CONFIG_MAX_FRAME_BUFFER_ATTACHMENTS);
	for(uint8_t i = 0; i < textureCount; ++i)
	{
		// Bind texture
		const std::shared_ptr<Texture>& texture = renderTarget->GetTexture(i);
		ASSERT(texture->renderData == nullptr);
		Bind(texture.get());

		const TextureRenderDataBgfx *textureRenderData = static_cast<const TextureRenderDataBgfx*>(texture->renderData);
		renderTargetTextures[i] = textureRenderData->textureHandle;
	}

	renderData->frameBuffer = bgfx::createFrameBuffer(textureCount, renderTargetTextures);
	bgfx::setViewFrameBuffer(renderData->id, renderData->frameBuffer);

	renderTarget->renderData = renderData;
}

void BgfxRender::Unbind(Mesh *mesh)
{
	if(mesh == nullptr || mesh->renderData == nullptr)
	{
		// Not yet bound
		return;
	}

	MeshRenderDataBgfx *renderData = static_cast<MeshRenderDataBgfx*>(mesh->renderData);

	switch(mesh->type)
	{
		case Mesh::EType::STATIC:
			bgfx::destroy(renderData->indexBuffer);
			bgfx::destroy(renderData->vertexBuffer);
			break;

		case Mesh::EType::DYNAMIC:
			bgfx::destroy(renderData->dynamicIndexBuffer);
			bgfx::destroy(renderData->dynamicVertexBuffer);
			break;
	}

	delete renderData;
	mesh->renderData = nullptr;
}

void BgfxRender::Unbind(Shader *shader)
{
	if(shader == nullptr || shader->renderData == nullptr)
	{
		// Not yet bound
		return;
	}

	ShaderRenderDataBgfx *renderData = static_cast<ShaderRenderDataBgfx*>(shader->renderData);
	bgfx::destroy(renderData->shaderHandle);

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

void BgfxRender::Unbind(Texture *texture)
{
	if(texture == nullptr || texture->renderData == nullptr)
	{
		// Not yet bound
		return;
	}

	TextureRenderDataBgfx *renderData = static_cast<TextureRenderDataBgfx*>(texture->renderData);
	bgfx::destroy(renderData->textureHandle);

	delete renderData;
	texture->renderData = nullptr;
}

void BgfxRender::Unbind(RenderTarget *renderTarget)
{
	if(renderTarget == nullptr || renderTarget->renderData == nullptr)
	{
		// Not yet bound
		return;
	}

	RenderTargetRenderDataBgfx *renderData = static_cast<RenderTargetRenderDataBgfx*>(renderTarget->renderData);

	bgfx::destroy(renderData->frameBuffer);

	const uint8_t textureCount = std::min<uint8_t>(renderTarget->GetTextureCount(), BGFX_CONFIG_MAX_FRAME_BUFFER_ATTACHMENTS);
	for(uint8_t i = 0; i < textureCount; ++i)
	{
		Unbind(renderTarget->GetTexture(i).get());
	}

	delete renderData;
	renderTarget->renderData = nullptr;
}

void BgfxRender::UpdateDynamicMesh(DynamicMesh *mesh)
{
	if(mesh == nullptr || mesh->renderData == nullptr)
	{
		// Not yet bound
		mesh->isIndexDataUpdateRequired = false;
		mesh->isVertexDataUpdateRequired = false;
		return;
	}

	MeshRenderDataBgfx *renderData = static_cast<MeshRenderDataBgfx*>(mesh->renderData);

	if(mesh->isIndexDataUpdateRequired)
	{
		bgfx::updateDynamicIndexBuffer(renderData->dynamicIndexBuffer, 0, bgfx::copy(mesh->indices.get(), mesh->GetSizeOfIndices()));
		mesh->isIndexDataUpdateRequired = false;
	}

	if(mesh->isVertexDataUpdateRequired)
	{
		bgfx::updateDynamicVertexBuffer(renderData->dynamicVertexBuffer, 0, bgfx::copy(mesh->vertices.get(), mesh->GetSizeOfVertices()));
		mesh->isVertexDataUpdateRequired = false;
	}
}

void BgfxRender::SetViewport(uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
	bgfx::setViewRect(activeViewId, x, y, width, height);
}

void BgfxRender::SetMesh(Mesh *mesh)
{
	MeshRenderDataBgfx *renderData = static_cast<MeshRenderDataBgfx*>(mesh->renderData);

	switch(mesh->type)
	{
		case Mesh::EType::STATIC:
		{
			bgfx::setIndexBuffer(renderData->indexBuffer, renderData->indexOffset, mesh->numIndices);
			bgfx::setVertexBuffer(0, renderData->vertexBuffer, renderData->vertexOffset, mesh->numVertices);
			break;
		}

		case Mesh::EType::DYNAMIC:
		{
			const DynamicMesh *dynamicMesh = static_cast<const DynamicMesh*>(mesh);
			bgfx::setIndexBuffer(renderData->dynamicIndexBuffer, dynamicMesh->indexOffset + renderData->indexOffset, mesh->numIndices);
			bgfx::setVertexBuffer(0, renderData->dynamicVertexBuffer, dynamicMesh->vertexOffset + renderData->vertexOffset, mesh->numVertices);
			break;
		}
	}
}

void BgfxRender::SetScreenSpaceQuadMesh()
{
	bgfx::TransientIndexBuffer indexBuffer;
	bgfx::TransientVertexBuffer vertexBuffer;
	if(bgfx::allocTransientBuffers(&vertexBuffer, screenSpaceQuadMeshVertexDecl, 4, &indexBuffer, 6))
	{
		// Vertices
		const float vertices[] =
		{
			-1.0f,	 1.0f,
			1.0f,	 1.0f,
			-1.0f,	-1.0f,
			1.0f,	-1.0f,
		};
		memcpy(vertexBuffer.data, vertices, sizeof(vertices));

		// Indices
		uint16_t *indices = (uint16_t*)indexBuffer.data;
		indices[0] = 0;
		indices[1] = 1;
		indices[2] = 2;

		indices[3] = 3;
		indices[4] = 2;
		indices[5] = 1;
	}

	bgfx::setIndexBuffer(&indexBuffer);
	bgfx::setVertexBuffer(0, &vertexBuffer);
}

void BgfxRender::SetVertexShader(Shader *vertexShader)
{
	activeVertexShader = vertexShader;
}

void BgfxRender::SetFragmentShader(Shader *fragmentShader)
{
	activeFragmentShader = fragmentShader;
}

void BgfxRender::SetTexture(uint8_t samplerIdx, Texture *texture, EFilterMode filterMode, EAddressMode addressMode)
{
	uint32_t flags = BGFX_TEXTURE_NONE;
	switch(filterMode)
	{
		case EFilterMode::POINT:		flags |= BGFX_TEXTURE_MIN_POINT | BGFX_TEXTURE_MAG_POINT | BGFX_TEXTURE_MIP_POINT; break;
		case EFilterMode::BILINEAR:		flags |= BGFX_TEXTURE_MIP_POINT; break;
		case EFilterMode::TRILINEAR:	break;
		case EFilterMode::ANISOTROPIC:	flags |= BGFX_TEXTURE_MIN_ANISOTROPIC | BGFX_TEXTURE_MAG_ANISOTROPIC; break;
	}
	switch(addressMode)
	{
		case EAddressMode::REPEAT:			break;
		case EAddressMode::CLAMP:			flags |= BGFX_TEXTURE_U_CLAMP | BGFX_TEXTURE_V_CLAMP | BGFX_TEXTURE_W_CLAMP; break;
		case EAddressMode::MIRRORED_REPEAT:	flags |= BGFX_TEXTURE_U_MIRROR | BGFX_TEXTURE_V_MIRROR | BGFX_TEXTURE_W_MIRROR; break;
		case EAddressMode::MIRROR_ONCE:		flags |= BGFX_TEXTURE_U_MIRROR | BGFX_TEXTURE_V_MIRROR | BGFX_TEXTURE_W_MIRROR; break;
		case EAddressMode::BORDER:			flags |= BGFX_TEXTURE_U_BORDER | BGFX_TEXTURE_V_BORDER | BGFX_TEXTURE_W_BORDER; break;
	}

	const TextureRenderDataBgfx *renderData = static_cast<const TextureRenderDataBgfx*>(texture->renderData);

	bgfx::setTexture(samplerIdx, samplerUniforms[samplerIdx], renderData->textureHandle, flags);
}

void BgfxRender::UpdateShaderParams(const Material *material)
{
	const ShaderRenderDataBgfx *vertexShaderRenderData = static_cast<const ShaderRenderDataBgfx*>(activeVertexShader->renderData);
	const ShaderRenderDataBgfx *fragmentShaderRenderData = static_cast<const ShaderRenderDataBgfx*>(activeFragmentShader->renderData);

	for(const Material::ShaderParam& shaderParam : material->GetShaderParams())
	{
		const bgfx::UniformHandle *uniform = vertexShaderRenderData->uniforms.Find(shaderParam.name);
		if(uniform == nullptr)
		{
			uniform = fragmentShaderRenderData->uniforms.Find(shaderParam.name);
		}

		if(uniform != nullptr && bgfx::isValid(*uniform))
		{
			const void *value = shaderParam.GetValue();
			bgfx::setUniform(*uniform, value);
		}
	}
}

void BgfxRender::DoRender()
{
	bgfx::setState(renderState, blendFactor);

	bgfx::ProgramHandle shaderProgram = BGFX_INVALID_HANDLE;

	const std::pair<uint64_t, uint64_t> key = std::make_pair((uint64_t)activeVertexShader->renderData, (uint64_t)activeFragmentShader->renderData);
	auto it = shaderProgramCache.find(key);
	if(it != shaderProgramCache.end())
	{
		shaderProgram = it->second;
	}
	else
	{
		const ShaderRenderDataBgfx *vertexShaderRenderData = static_cast<ShaderRenderDataBgfx*>(activeVertexShader->renderData);
		const ShaderRenderDataBgfx *fragmentShaderRenderData = static_cast<const ShaderRenderDataBgfx*>(activeFragmentShader->renderData);
		shaderProgram = bgfx::createProgram(vertexShaderRenderData->shaderHandle, fragmentShaderRenderData->shaderHandle);

		shaderProgramCache.insert(std::make_pair(key, shaderProgram));
	}

	bgfx::submit(activeViewId, shaderProgram);
}

bgfx::TextureFormat::Enum BgfxRender::ConvertTextureFormat(Texture::EFormat textureFormat)
{
	switch(textureFormat)
	{
		case Texture::EFormat::UNKNOWN:		return bgfx::TextureFormat::Unknown;
		case Texture::EFormat::R8:			return bgfx::TextureFormat::R8;
		case Texture::EFormat::RG8:			return bgfx::TextureFormat::RG8;
		case Texture::EFormat::RGB8:		return bgfx::TextureFormat::RGB8;
		case Texture::EFormat::RGBA8:		return bgfx::TextureFormat::RGBA8;
		case Texture::EFormat::RGBA32F:		return bgfx::TextureFormat::RGBA32F;
		case Texture::EFormat::D24S8:		return bgfx::TextureFormat::D24S8;
	}

	return bgfx::TextureFormat::Unknown;
}

void BgfxRender::BindEmbeddedShader(Shader *shader, const char *name)
{
	ASSERT(shader != nullptr);

	if(shader->renderData != nullptr)
	{
		// Already bound
		return;
	}

	ShaderRenderDataBgfx *renderData = new ShaderRenderDataBgfx();
	renderData->shaderHandle = bgfx::createEmbeddedShader(s_embeddedShaders, bgfx::getRendererType(), "vs_screenSpaceQuad");
	ASSERT(bgfx::isValid(renderData->shaderHandle));

	shader->renderData = renderData;
}
