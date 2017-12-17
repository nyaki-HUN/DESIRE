#include "stdafx.h"
#include "RenderBgfx.h"
#include "MeshRenderDataBgfx.h"
#include "ShaderRenderDataBgfx.h"
#include "RenderTargetRenderDataBgfx.h"

#include "Core/IWindow.h"
#include "Core/String.h"
#include "Core/fs/FileSystem.h"
#include "Core/fs/IReadFile.h"
#include "Core/math/vectormath.h"
#include "Render/Material.h"
#include "Render/RenderTarget.h"
#include "Render/View.h"
#include "Resource/Mesh.h"
#include "Resource/Shader.h"

#include "bgfx/platform.h"
#include "bgfx/../../src/config.h"

RenderBgfx::RenderBgfx()
{
	for(bgfx::UniformHandle& uniform : samplerUniforms)
	{
		uniform = BGFX_INVALID_HANDLE;
	}
}

RenderBgfx::~RenderBgfx()
{

}

void RenderBgfx::Init(IWindow *mainWindow)
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

	for(size_t i = 0; i < DESIRE_ASIZEOF(samplerUniforms); ++i)
	{
		samplerUniforms[i] = bgfx::createUniform("s_tex", bgfx::UniformType::Int1);
	}

	renderState = BGFX_STATE_MSAA;
	SetDefaultRenderStates();

	bgfx::reset(mainWindow->GetWidth(), mainWindow->GetHeight(), BGFX_RESET_VSYNC);
}

void RenderBgfx::UpdateRenderWindow(IWindow *window)
{
	if(!initialized)
	{
		return;
	}

	bgfx::reset(window->GetWidth(), window->GetHeight(), BGFX_RESET_VSYNC);
}

void RenderBgfx::Kill()
{
	for(bgfx::UniformHandle& uniform : samplerUniforms)
	{
		bgfx::destroy(uniform);
		uniform = BGFX_INVALID_HANDLE;
	}

	bgfx::shutdown();
	initialized = false;
}

String RenderBgfx::GetShaderFilenameWithPath(const char *shaderFilename) const
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

void RenderBgfx::BeginFrame(IWindow *window)
{
	activeViewId = 0;
	SetViewport(0, 0, window->GetWidth(), window->GetHeight());

	// This dummy draw call is here to make sure that view 0 is cleared if no other draw calls are submitted to view 0
	bgfx::touch(0);
}

void RenderBgfx::EndFrame()
{
	bgfx::frame();
}

void RenderBgfx::SetView(View *view)
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

void RenderBgfx::SetWorldMatrix(const Matrix4& matrix)
{
	float world[16];
	matrix.Store(world);

	bgfx::setTransform(world);
}

void RenderBgfx::SetViewProjectionMatrices(const Matrix4& viewMatrix, const Matrix4& projMatrix)
{
	float view[16];
	float projection[16];
	viewMatrix.Store(view);
	projMatrix.Store(projection);

	bgfx::setViewTransform(activeViewId, view, projection);
}

void RenderBgfx::SetScissor(uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
	bgfx::setScissor(x, y, width, height);
}

void RenderBgfx::SetClearColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	clearColor = (r << 24) | (g << 16) | (b << 8) | a;
}

void RenderBgfx::SetColorWriteEnabled(bool rgbWriteEnabled, bool alphaWriteEnabled)
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

void RenderBgfx::SetDepthWriteEnabled(bool enabled)
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

void RenderBgfx::SetDepthTest(EDepthTest depthTest)
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

void RenderBgfx::SetCullMode(ECullMode cullMode)
{
	renderState &= ~BGFX_STATE_CULL_MASK;

	switch(cullMode)
	{
		case IRender::ECullMode::NONE:	break;
		case IRender::ECullMode::CCW:	renderState |= BGFX_STATE_CULL_CCW; break;
		case IRender::ECullMode::CW:	renderState |= BGFX_STATE_CULL_CW; break;
	}
}

void RenderBgfx::Bind(Mesh *mesh)
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
		bgfx::Attrib::TexCoord0,	// Mesh::EAttrib::TEXCOORD0
		bgfx::Attrib::TexCoord1,	// Mesh::EAttrib::TEXCOORD1
		bgfx::Attrib::TexCoord2,	// Mesh::EAttrib::TEXCOORD2
		bgfx::Attrib::Color0,		// Mesh::EAttrib::COLOR
	};
	DESIRE_CHECK_ARRAY_SIZE(attribConversionTable, Mesh::EAttrib);

	static const bgfx::AttribType::Enum attribTypeConversionTable[] =
	{
		bgfx::AttribType::Enum::Float,	// Mesh::EAttribType::FLOAT
		bgfx::AttribType::Enum::Uint8,	// Mesh::EAttribType::UINT8
	};
	DESIRE_CHECK_ARRAY_SIZE(attribTypeConversionTable, Mesh::EAttribType);

	renderData->vertexDecl.begin();
	for(Mesh::VertexDecl& decl : mesh->vertexDecl)
	{
		const bool isNormalized = (decl.type == Mesh::EAttribType::UINT8);
		renderData->vertexDecl.add(attribConversionTable[(size_t)decl.attrib], (uint8_t)decl.count, attribTypeConversionTable[(size_t)decl.type], isNormalized);
	}
	renderData->vertexDecl.end();

	switch(mesh->type)
	{
		case Mesh::EType::STATIC:
		{
			if(mesh->numIndices != 0)
			{
				const bgfx::Memory *indexData = bgfx::makeRef(mesh->indices, mesh->GetSizeOfIndices());
				renderData->indexBuffer = bgfx::createIndexBuffer(indexData, BGFX_BUFFER_NONE);
			}

			if(mesh->numVertices != 0)
			{
				const bgfx::Memory *vertexData = bgfx::makeRef(mesh->vertices, mesh->GetSizeOfVertices());
				renderData->vertexBuffer = bgfx::createVertexBuffer(vertexData, renderData->vertexDecl, BGFX_BUFFER_NONE);
			}
			break;
		}

		case Mesh::EType::DYNAMIC:
		{
			DynamicMesh *dynamicMesh = static_cast<DynamicMesh*>(mesh);

			if(dynamicMesh->maxNumOfIndices != 0)
			{
				const bgfx::Memory *indexData = bgfx::copy(dynamicMesh->indices, dynamicMesh->GetMaxSizeOfIndices());
				renderData->dynamicIndexBuffer = bgfx::createDynamicIndexBuffer(indexData, BGFX_BUFFER_NONE);
			}

			if(dynamicMesh->maxNumOfVertices != 0)
			{
				const bgfx::Memory *vertexData = bgfx::copy(dynamicMesh->vertices, dynamicMesh->GetMaxSizeOfVertices());
				renderData->dynamicVertexBuffer = bgfx::createDynamicVertexBuffer(vertexData, renderData->vertexDecl, BGFX_BUFFER_NONE);
			}
			break;
		}
	}

	mesh->renderData = renderData;
}

void RenderBgfx::Bind(Shader *shader)
{
	ASSERT(shader != nullptr);

	if(shader->renderData != nullptr)
	{
		// Already bound
		return;
	}
	
	ShaderRenderDataBgfx *renderData = new ShaderRenderDataBgfx();

	const bgfx::Memory *shaderDara = bgfx::makeRef(shader->data.data, (uint32_t)shader->data.size);

	renderData->shaderHandle = bgfx::createShader(shaderDara);
	bgfx::setName(renderData->shaderHandle, shader->name.c_str());

	renderData->u_tint = bgfx::createUniform("u_tint", bgfx::UniformType::Vec4);

	shader->renderData = renderData;
}

void RenderBgfx::Bind(Texture *texture)
{
	ASSERT(texture != nullptr);

	if(texture->renderData != nullptr)
	{
		// Already bound
		return;
	}

	bgfx::TextureHandle handle = bgfx::createTexture2D(texture->width, texture->height, (texture->numMipMaps != 0), 1, ConvertTextureFormat(texture->format), BGFX_TEXTURE_NONE, bgfx::makeRef(texture->data.data, (uint32_t)texture->data.size));

	texture->renderData = new bgfx::TextureHandle(handle);
}

void RenderBgfx::Bind(RenderTarget *renderTarget)
{
	ASSERT(renderTarget != nullptr);

	if(renderTarget->renderData != nullptr)
	{
		// Already bound
		return;
	}

	RenderTargetRenderDataBgfx *renderData = new RenderTargetRenderDataBgfx();

	const uint32_t flags = BGFX_TEXTURE_RT
		| BGFX_TEXTURE_MIN_POINT
		| BGFX_TEXTURE_MAG_POINT
		| BGFX_TEXTURE_MIP_POINT
		| BGFX_TEXTURE_U_CLAMP
		| BGFX_TEXTURE_V_CLAMP;

	bgfx::TextureHandle renderTargetTextures[BGFX_CONFIG_MAX_FRAME_BUFFER_ATTACHMENTS];
	const uint8_t textureCount = std::min<uint8_t>(renderTarget->GetTextureCount(), BGFX_CONFIG_MAX_FRAME_BUFFER_ATTACHMENTS);
	for(uint8_t i = 0; i < textureCount; ++i)
	{
		// Bind texture
		const std::shared_ptr<Texture>& texture = renderTarget->GetTexture(i);
		ASSERT(texture->renderData == nullptr);

		bgfx::TextureHandle handle = bgfx::createTexture2D(texture->width, texture->height, (texture->numMipMaps != 0), 1, ConvertTextureFormat(texture->format), flags);
		texture->renderData = new bgfx::TextureHandle(handle);

		renderTargetTextures[i] = handle;
	}

	renderData->frameBuffer = bgfx::createFrameBuffer(textureCount, renderTargetTextures);
	bgfx::setViewFrameBuffer(renderData->id, renderData->frameBuffer);

	renderTarget->renderData = renderData;
}

void RenderBgfx::Unbind(Mesh *mesh)
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

void RenderBgfx::Unbind(Shader *shader)
{
	if(shader == nullptr || shader->renderData == nullptr)
	{
		// Not yet bound
		return;
	}

	ShaderRenderDataBgfx *renderData = static_cast<ShaderRenderDataBgfx*>(shader->renderData);

	bgfx::destroy(renderData->shaderHandle);
	bgfx::destroy(renderData->u_tint);

	delete renderData;
	shader->renderData = nullptr;
}

void RenderBgfx::Unbind(Texture *texture)
{
	if(texture == nullptr || texture->renderData == nullptr)
	{
		// Not yet bound
		return;
	}

	bgfx::TextureHandle *renderData = static_cast<bgfx::TextureHandle*>(texture->renderData);
	bgfx::destroy(*renderData);

	delete renderData;
	texture->renderData = nullptr;
}

void RenderBgfx::Unbind(RenderTarget *renderTarget)
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

void RenderBgfx::UpdateDynamicMesh(DynamicMesh *mesh)
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
		bgfx::updateDynamicIndexBuffer(renderData->dynamicIndexBuffer, 0, bgfx::copy(mesh->indices, mesh->GetSizeOfIndices()));
		mesh->isIndexDataUpdateRequired = false;
	}

	if(mesh->isVertexDataUpdateRequired)
	{
		bgfx::updateDynamicVertexBuffer(renderData->dynamicVertexBuffer, 0, bgfx::copy(mesh->vertices, mesh->GetSizeOfVertices()));
		mesh->isVertexDataUpdateRequired = false;
	}
}

void RenderBgfx::SetViewport(uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
	bgfx::setViewRect(activeViewId, x, y, width, height);
}

void RenderBgfx::SetMesh(Mesh *mesh)
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

void RenderBgfx::SetShadersFromMaterial(Material *material)
{
	ShaderRenderDataBgfx *vertexShaderRenderData = static_cast<ShaderRenderDataBgfx*>(material->vertexShader->renderData);
//	const ShaderRenderDataBgfx *vertexShaderRenderData = static_cast<const ShaderRenderDataBgfx*>(material->vertexShader->renderData);
	const ShaderRenderDataBgfx *pixelShaderRenderData = static_cast<const ShaderRenderDataBgfx*>(material->pixelShader->renderData);

	if(!bgfx::isValid(vertexShaderRenderData->shaderProgram))
	{
		vertexShaderRenderData->shaderProgram = bgfx::createProgram(vertexShaderRenderData->shaderHandle, pixelShaderRenderData->shaderHandle);
	}
	activeShaderProgram = vertexShaderRenderData->shaderProgram;

	const float color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
//	const float colorHighlighted[4] = { 0.3f, 0.3f, 2.0f, 1.0f };
	bgfx::setUniform(pixelShaderRenderData->u_tint, color);

	// TODO: proper render state handling
/**/if(activeViewId == 0)
	{
		renderState |= BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA);
	}
	else
	{
		renderState &= ~BGFX_STATE_BLEND_MASK;
	}
}

void RenderBgfx::SetTexture(uint8_t samplerIdx, Texture *texture)
{
	const bgfx::TextureHandle *renderData = static_cast<const bgfx::TextureHandle*>(texture->renderData);
	bgfx::setTexture(samplerIdx, samplerUniforms[samplerIdx], *renderData);
}

void RenderBgfx::DoRender()
{
	bgfx::setState(renderState, blendFactor);
	bgfx::submit(activeViewId, activeShaderProgram);
}

bgfx::TextureFormat::Enum RenderBgfx::ConvertTextureFormat(Texture::EFormat textureFormat)
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
