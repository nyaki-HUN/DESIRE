#include "BgfxRender.h"
#include "MeshRenderDataBgfx.h"
#include "ShaderRenderDataBgfx.h"
#include "RenderTargetRenderDataBgfx.h"
#include "TextureRenderDataBgfx.h"
#include "Embedded_shaders/vs_screenSpaceQuad.bin.h"

#include "Engine/Application/OSWindow.h"
#include "Engine/Core/assert.h"
#include "Engine/Core/math/Matrix4.h"
#include "Engine/Core/String/StackString.h"
#include "Engine/Render/Material.h"
#include "Engine/Render/RenderTarget.h"
#include "Engine/Render/View.h"
#include "Engine/Resource/Mesh.h"
#include "Engine/Resource/Shader.h"
#include "Engine/Resource/Texture.h"

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

	BindEmbeddedShader(screenSpaceQuadVertexShader.get(), "vs_screenSpaceQuad");

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

	activeVertexShader = nullptr;
	activeFragmentShader = nullptr;

	Unbind(screenSpaceQuadVertexShader.get());

	bgfx::shutdown();
	initialized = false;
}

DynamicString BgfxRender::GetShaderFilenameWithPath(const String& shaderFilename) const
{
	DynamicString filenameWithPath = "data/shaders/bgfx/";

	switch(bgfx::getRendererType())
	{
		case bgfx::RendererType::Direct3D9:		filenameWithPath += "dx9/"; break;
		case bgfx::RendererType::Direct3D11:	filenameWithPath += "dx11/"; break;
		case bgfx::RendererType::Direct3D12:	filenameWithPath += "dx11/"; break;
		case bgfx::RendererType::Gnm:			filenameWithPath += "pssl/"; break;
		case bgfx::RendererType::Metal:			filenameWithPath += "metal/"; break;
		case bgfx::RendererType::OpenGLES:		filenameWithPath += "essl/"; break;
		case bgfx::RendererType::OpenGL:		filenameWithPath += "glsl/"; break;
		case bgfx::RendererType::Vulkan:		filenameWithPath += "spirv/"; break;
		default:								ASSERT(false && "Not supported renderer type"); break;
	}

	filenameWithPath += shaderFilename;
	filenameWithPath += ".bin";
	return filenameWithPath;
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
		if(rt->renderData == nullptr)
		{
			Bind(rt);
		}

		RenderTargetRenderDataBgfx* renderData = static_cast<RenderTargetRenderDataBgfx*>(rt->renderData);
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

void BgfxRender::Bind(Mesh* mesh)
{
	ASSERT(mesh != nullptr);

	if(mesh->renderData != nullptr)
	{
		// Already bound
		return;
	}

	MeshRenderDataBgfx* renderData = new MeshRenderDataBgfx();

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

	renderData->vertexDecl.begin();
	for(Mesh::VertexDecl& decl : mesh->vertexDecl)
	{
		const bool isNormalized = (decl.type == Mesh::EAttribType::Uint8);
		renderData->vertexDecl.add(s_attribConversionTable[(size_t)decl.attrib], decl.count, s_attribTypeConversionTable[(size_t)decl.type], isNormalized);
	}
	renderData->vertexDecl.end();

	switch(mesh->type)
	{
		case Mesh::EType::Static:
		{
			if(mesh->numIndices != 0)
			{
				const bgfx::Memory* indexData = bgfx::makeRef(mesh->indices.get(), mesh->GetSizeOfIndices());
				renderData->indexBuffer = bgfx::createIndexBuffer(indexData, BGFX_BUFFER_NONE);
			}

			if(mesh->numVertices != 0)
			{
				const bgfx::Memory* vertexData = bgfx::makeRef(mesh->vertices.get(), mesh->GetSizeOfVertices());
				renderData->vertexBuffer = bgfx::createVertexBuffer(vertexData, renderData->vertexDecl, BGFX_BUFFER_NONE);
			}
			break;
		}

		case Mesh::EType::Dynamic:
		{
			DynamicMesh* dynamicMesh = static_cast<DynamicMesh*>(mesh);

			if(dynamicMesh->maxNumOfIndices != 0)
			{
				const bgfx::Memory* indexData = bgfx::copy(dynamicMesh->indices.get(), dynamicMesh->GetMaxSizeOfIndices());
				renderData->dynamicIndexBuffer = bgfx::createDynamicIndexBuffer(indexData, BGFX_BUFFER_NONE);
			}

			if(dynamicMesh->maxNumOfVertices != 0)
			{
				const bgfx::Memory* vertexData = bgfx::copy(dynamicMesh->vertices.get(), dynamicMesh->GetMaxSizeOfVertices());
				renderData->dynamicVertexBuffer = bgfx::createDynamicVertexBuffer(vertexData, renderData->vertexDecl, BGFX_BUFFER_NONE);
			}
			break;
		}
	}

	mesh->renderData = renderData;
}

void BgfxRender::Bind(Shader* shader)
{
	ASSERT(shader != nullptr);

	if(shader->renderData != nullptr)
	{
		// Already bound
		return;
	}

	ShaderRenderDataBgfx* renderData = new ShaderRenderDataBgfx();

	const bgfx::Memory* shaderData = nullptr;
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
		uint8_t* ptr = shaderData->data;
		for(const String& define : shader->defines)
		{
			memcpy(ptr, "#define ", 8);
			ptr += 8;
			memcpy(ptr, define.Str(), define.Length());
			ptr += define.Length();
			memcpy(ptr, " 1\n", 3);
			ptr += 3;
		}
		memcpy(ptr, shader->data.data, shader->data.size);
	}

	renderData->shaderHandle = bgfx::createShader(shaderData);
	ASSERT(bgfx::isValid(renderData->shaderHandle));
	bgfx::setName(renderData->shaderHandle, shader->name.Str());

	const uint16_t uniformCount = bgfx::getShaderUniforms(renderData->shaderHandle, nullptr, 0);
	bgfx::UniformHandle* uniforms = static_cast<bgfx::UniformHandle*>(alloca(uniformCount * sizeof(bgfx::UniformHandle)));
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

void BgfxRender::Bind(Texture* texture)
{
	ASSERT(texture != nullptr);

	if(texture->renderData != nullptr)
	{
		// Already bound
		return;
	}

	TextureRenderDataBgfx* renderData = new TextureRenderDataBgfx();

	const bool isRenderTarget = (texture->data.data == nullptr);

	if(isRenderTarget)
	{
		renderData->textureHandle = bgfx::createTexture2D(texture->width, texture->height, (texture->numMipMaps != 0), 1, GetTextureFormat(texture), BGFX_TEXTURE_RT);
	}
	else
	{
		renderData->textureHandle = bgfx::createTexture2D(texture->width, texture->height, (texture->numMipMaps != 0), 1, GetTextureFormat(texture), BGFX_TEXTURE_NONE, bgfx::makeRef(texture->data.data, (uint32_t)texture->data.size));
	}

	texture->renderData = renderData;
}

void BgfxRender::Bind(RenderTarget* renderTarget)
{
	ASSERT(renderTarget != nullptr);

	if(renderTarget->renderData != nullptr)
	{
		// Already bound
		return;
	}

	RenderTargetRenderDataBgfx* renderData = new RenderTargetRenderDataBgfx();

	bgfx::TextureHandle renderTargetTextures[BGFX_CONFIG_MAX_FRAME_BUFFER_ATTACHMENTS];
	const uint8_t textureCount = std::min<uint8_t>(renderTarget->GetTextureCount(), BGFX_CONFIG_MAX_FRAME_BUFFER_ATTACHMENTS);
	for(uint8_t i = 0; i < textureCount; ++i)
	{
		// Bind texture
		const std::shared_ptr<Texture>& texture = renderTarget->GetTexture(i);
		ASSERT(texture->renderData == nullptr);
		Bind(texture.get());

		const TextureRenderDataBgfx* textureRenderData = static_cast<const TextureRenderDataBgfx*>(texture->renderData);
		renderTargetTextures[i] = textureRenderData->textureHandle;
	}

	renderData->frameBuffer = bgfx::createFrameBuffer(textureCount, renderTargetTextures);
	bgfx::setViewFrameBuffer(renderData->id, renderData->frameBuffer);

	renderTarget->renderData = renderData;
}

void BgfxRender::Unbind(Mesh* mesh)
{
	if(mesh == nullptr || mesh->renderData == nullptr)
	{
		// Not yet bound
		return;
	}

	MeshRenderDataBgfx* renderData = static_cast<MeshRenderDataBgfx*>(mesh->renderData);

	switch(mesh->type)
	{
		case Mesh::EType::Static:
			bgfx::destroy(renderData->indexBuffer);
			bgfx::destroy(renderData->vertexBuffer);
			break;

		case Mesh::EType::Dynamic:
			bgfx::destroy(renderData->dynamicIndexBuffer);
			bgfx::destroy(renderData->dynamicVertexBuffer);
			break;
	}

	delete renderData;
	mesh->renderData = nullptr;
}

void BgfxRender::Unbind(Shader* shader)
{
	if(shader == nullptr || shader->renderData == nullptr)
	{
		// Not yet bound
		return;
	}

	ShaderRenderDataBgfx* renderData = static_cast<ShaderRenderDataBgfx*>(shader->renderData);
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

void BgfxRender::Unbind(Texture* texture)
{
	if(texture == nullptr || texture->renderData == nullptr)
	{
		// Not yet bound
		return;
	}

	TextureRenderDataBgfx* renderData = static_cast<TextureRenderDataBgfx*>(texture->renderData);
	bgfx::destroy(renderData->textureHandle);

	delete renderData;
	texture->renderData = nullptr;
}

void BgfxRender::Unbind(RenderTarget* renderTarget)
{
	if(renderTarget == nullptr || renderTarget->renderData == nullptr)
	{
		// Not yet bound
		return;
	}

	RenderTargetRenderDataBgfx* renderData = static_cast<RenderTargetRenderDataBgfx*>(renderTarget->renderData);

	bgfx::destroy(renderData->frameBuffer);

	const uint8_t textureCount = std::min<uint8_t>(renderTarget->GetTextureCount(), BGFX_CONFIG_MAX_FRAME_BUFFER_ATTACHMENTS);
	for(uint8_t i = 0; i < textureCount; ++i)
	{
		Unbind(renderTarget->GetTexture(i).get());
	}

	delete renderData;
	renderTarget->renderData = nullptr;
}

void BgfxRender::UpdateDynamicMesh(DynamicMesh* mesh)
{
	ASSERT(mesh != nullptr);

	if(mesh->renderData == nullptr)
	{
		// Not yet bound
		mesh->isIndexDataUpdateRequired = false;
		mesh->isVertexDataUpdateRequired = false;
		return;
	}

	MeshRenderDataBgfx* renderData = static_cast<MeshRenderDataBgfx*>(mesh->renderData);

	if(mesh->isIndexDataUpdateRequired)
	{
		bgfx::update(renderData->dynamicIndexBuffer, 0, bgfx::copy(mesh->indices.get(), mesh->GetSizeOfIndices()));
		mesh->isIndexDataUpdateRequired = false;
	}

	if(mesh->isVertexDataUpdateRequired)
	{
		bgfx::update(renderData->dynamicVertexBuffer, 0, bgfx::copy(mesh->vertices.get(), mesh->GetSizeOfVertices()));
		mesh->isVertexDataUpdateRequired = false;
	}
}

void BgfxRender::SetViewport(uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
	bgfx::setViewRect(activeViewId, x, y, width, height);
}

void BgfxRender::SetMesh(Mesh* mesh)
{
	MeshRenderDataBgfx* renderData = static_cast<MeshRenderDataBgfx*>(mesh->renderData);

	switch(mesh->type)
	{
		case Mesh::EType::Static:
		{
			bgfx::setIndexBuffer(renderData->indexBuffer, renderData->indexOffset, mesh->numIndices);
			bgfx::setVertexBuffer(0, renderData->vertexBuffer, renderData->vertexOffset, mesh->numVertices);
			break;
		}

		case Mesh::EType::Dynamic:
		{
			const DynamicMesh* dynamicMesh = static_cast<const DynamicMesh*>(mesh);
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
		uint16_t* indices = (uint16_t*)indexBuffer.data;
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

void BgfxRender::SetVertexShader(Shader* vertexShader)
{
	activeVertexShader = vertexShader;
}

void BgfxRender::SetFragmentShader(Shader* fragmentShader)
{
	activeFragmentShader = fragmentShader;
}

void BgfxRender::SetTexture(uint8_t samplerIdx, Texture* texture, EFilterMode filterMode, EAddressMode addressMode)
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

	const TextureRenderDataBgfx* renderData = static_cast<const TextureRenderDataBgfx*>(texture->renderData);

	bgfx::setTexture(samplerIdx, samplerUniforms[samplerIdx], renderData->textureHandle, flags);
}

void BgfxRender::UpdateShaderParams(const Material* material)
{
	const ShaderRenderDataBgfx* vertexShaderRenderData = static_cast<const ShaderRenderDataBgfx*>(activeVertexShader->renderData);
	const ShaderRenderDataBgfx* fragmentShaderRenderData = static_cast<const ShaderRenderDataBgfx*>(activeFragmentShader->renderData);

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
		const ShaderRenderDataBgfx* vertexShaderRenderData = static_cast<ShaderRenderDataBgfx*>(activeVertexShader->renderData);
		const ShaderRenderDataBgfx* fragmentShaderRenderData = static_cast<const ShaderRenderDataBgfx*>(activeFragmentShader->renderData);
		shaderProgram = bgfx::createProgram(vertexShaderRenderData->shaderHandle, fragmentShaderRenderData->shaderHandle);

		shaderProgramCache.insert(std::make_pair(key, shaderProgram));
	}

	bgfx::submit(activeViewId, shaderProgram);
}

bgfx::TextureFormat::Enum BgfxRender::GetTextureFormat(const Texture* texture)
{
	const bgfx::TextureFormat::Enum conversionTable[] =
	{
		bgfx::TextureFormat::Unknown,		// Texture::EFormat::Unknown
		bgfx::TextureFormat::R8,			// Texture::EFormat::R8
		bgfx::TextureFormat::RG8,			// Texture::EFormat::RG8
		bgfx::TextureFormat::RGB8,			// Texture::EFormat::RGB8
		bgfx::TextureFormat::RGBA8,			// Texture::EFormat::RGBA8
		bgfx::TextureFormat::RGBA32F,		// Texture::EFormat::RGBA32F
		bgfx::TextureFormat::D16,			// Texture::EFormat::D16
		bgfx::TextureFormat::D24S8,			// Texture::EFormat::D24_S8
		bgfx::TextureFormat::D32,			// Texture::EFormat::D32
	};
	DESIRE_CHECK_ARRAY_SIZE(conversionTable, Texture::EFormat::D32 + 1);

	return conversionTable[(size_t)texture->format];
}

void BgfxRender::BindEmbeddedShader(Shader* shader, const char* name)
{
	ASSERT(shader != nullptr);

	if(shader->renderData != nullptr)
	{
		// Already bound
		return;
	}

	ShaderRenderDataBgfx* renderData = new ShaderRenderDataBgfx();
	renderData->shaderHandle = bgfx::createEmbeddedShader(s_embeddedShaders, bgfx::getRendererType(), "vs_screenSpaceQuad");
	ASSERT(bgfx::isValid(renderData->shaderHandle));

	shader->renderData = renderData;
}
