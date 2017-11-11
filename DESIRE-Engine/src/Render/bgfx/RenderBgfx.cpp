#include "stdafx.h"
#include "Render/bgfx/RenderBgfx.h"
#include "Render/bgfx/MeshRenderDataBgfx.h"
#include "Render/bgfx/ShaderRenderDataBgfx.h"
#include "Render/Material.h"
#include "Render/View.h"
#include "Core/IWindow.h"
#include "Core/String.h"
#include "Core/fs/FileSystem.h"
#include "Core/fs/IReadFile.h"
#include "Core/math/vectormath.h"
#include "Resource/Mesh.h"
#include "Resource/Shader.h"
#include "Resource/Texture.h"

#include "bgfx/platform.h"

RenderBgfx::RenderBgfx()
	: activeShaderProgram(BGFX_INVALID_HANDLE)
	, activeViewId(0)
	, initialized(false)
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
	bgfx::setViewClear(activeViewId, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x303030ff, 1.0f, 0);

	for(size_t i = 0; i < DESIRE_ASIZEOF(samplerUniforms); ++i)
	{
		samplerUniforms[i] = bgfx::createUniform("s_tex", bgfx::UniformType::Int1);
	}

	UpdateRenderWindow(mainWindow);
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
		activeViewId = view->GetID();
		SetViewport(view->GetPosX(), view->GetPosY(), view->GetWidth(), view->GetHeight());
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

	bgfx::TextureFormat::Enum format = bgfx::TextureFormat::Unknown;
	switch(texture->format)
	{
		case Texture::EFormat::UNKNOWN:		format = bgfx::TextureFormat::Unknown; break;
		case Texture::EFormat::R8:			format = bgfx::TextureFormat::R8; break;
		case Texture::EFormat::RG8:			format = bgfx::TextureFormat::RG8; break;
		case Texture::EFormat::RGB8:		format = bgfx::TextureFormat::RGB8; break;
		case Texture::EFormat::RGBA8:		format = bgfx::TextureFormat::RGBA8; break;
		case Texture::EFormat::RGBA32F:		format = bgfx::TextureFormat::RGBA32F; break;
	}

	bgfx::TextureHandle handle = bgfx::createTexture2D(texture->width, texture->height, (texture->numMipMaps != 0), 1, format, BGFX_TEXTURE_NONE, bgfx::makeRef(texture->data.data, (uint32_t)texture->data.size));

	texture->renderData = new bgfx::TextureHandle(handle);
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

void RenderBgfx::UpdateDynamicMesh(DynamicMesh *mesh)
{
	if(mesh == nullptr || mesh->renderData == nullptr)
	{
		// Not yet bound
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
		bgfx::setState(activeViewId
			| BGFX_STATE_RGB_WRITE
			| BGFX_STATE_ALPHA_WRITE
			| BGFX_STATE_DEPTH_TEST_LESS
			| BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA)
			| BGFX_STATE_MSAA
		);
	}
}

void RenderBgfx::SetTexture(uint8_t samplerIdx, Texture *texture)
{
	const bgfx::TextureHandle *renderData = static_cast<const bgfx::TextureHandle*>(texture->renderData);
	bgfx::setTexture(samplerIdx, samplerUniforms[samplerIdx], *renderData);
}

void RenderBgfx::SetViewport(uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
	bgfx::setViewRect(activeViewId, x, y, width, height);
}

void RenderBgfx::DoRender()
{
	bgfx::submit(activeViewId, activeShaderProgram);
}

