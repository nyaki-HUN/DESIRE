#include "stdafx.h"
#include "Render/bgfx/RenderBgfx.h"
#include "Render/bgfx/MeshRenderDataBgfx.h"
#include "Core/IWindow.h"
#include "Core/String.h"
#include "Core/fs/FileSystem.h"
#include "Core/fs/IReadFile.h"
#include "Resource/Mesh.h"
#include "Resource/Texture.h"

#include "bgfx/platform.h"

RenderBgfx::RenderBgfx()
	: initialized(false)
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
	bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x303030ff, 1.0f, 0);

	for(size_t i = 0; i < DESIRE_ASIZEOF(samplerUniforms); ++i)
	{
		samplerUniforms[i] = bgfx::createUniform("s_tex", bgfx::UniformType::Int1);
	}

	UpdateRenderWindow(mainWindow);
}

void RenderBgfx::Kill()
{
	for(bgfx::UniformHandle& uniform : samplerUniforms)
	{
		bgfx::destroyUniform(uniform);
		uniform = BGFX_INVALID_HANDLE;
	}

	bgfx::shutdown();
	initialized = false;
}

void RenderBgfx::UpdateRenderWindow(IWindow *window)
{
	if(!initialized)
	{
		return;
	}

	bgfx::reset(window->GetWidth(), window->GetHeight(), BGFX_RESET_VSYNC);
}

void RenderBgfx::BeginFrame(IWindow *window)
{
	bgfx::setViewRect(0, 0, 0, window->GetWidth(), window->GetHeight());

	// This dummy draw call is here to make sure that view 0 is cleared if no other draw calls are submitted to view 0
	bgfx::touch(0);
}

void RenderBgfx::EndFrame()
{
	bgfx::frame();
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

/*	renderData->vertexDecl.begin()
		.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
		.add(bgfx::Attrib::Normal, 3, bgfx::AttribType::Float)
		.add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
		.end();
*/
	renderData->vertexDecl.begin()
		.add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float)
		.add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
		.add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
		.end();

	const bgfx::Memory *indexData = bgfx::makeRef(mesh->indices, mesh->GetSizeOfIndices());
	const bgfx::Memory *vertexData = bgfx::makeRef(mesh->vertices, mesh->GetSizeOfVertices());

	renderData->indexBuffer = bgfx::createIndexBuffer(indexData, BGFX_BUFFER_NONE);
	renderData->vertexBuffer = bgfx::createVertexBuffer(vertexData, renderData->vertexDecl, BGFX_BUFFER_NONE);

	mesh->renderData = renderData;
}

void RenderBgfx::Unbind(Mesh *mesh)
{
	ASSERT(mesh != nullptr);

	if(mesh->renderData == nullptr)
	{
		// Not yet bound
		return;
	}

	MeshRenderDataBgfx *renderData = static_cast<MeshRenderDataBgfx*>(mesh->renderData);
	bgfx::destroyIndexBuffer(renderData->indexBuffer);
	bgfx::destroyVertexBuffer(renderData->vertexBuffer);

	delete renderData;
	mesh->renderData = nullptr;
}

void RenderBgfx::SetMesh(Mesh *mesh)
{
	if(mesh->renderData == nullptr)
	{
		Bind(mesh);
	}

	MeshRenderDataBgfx *renderData = static_cast<MeshRenderDataBgfx*>(mesh->renderData);

	// Update buffers
	if(mesh->isUpdateRequired)
	{
		switch(mesh->type)
		{
			case Mesh::EType::STATIC:
				// No update for static mesh
				break;

			case Mesh::EType::DYNAMIC:
				break;

			case Mesh::EType::TRANSIENT:
			{
				if(bgfx::getAvailTransientIndexBuffer(mesh->numIndices) != mesh->numIndices || bgfx::getAvailTransientVertexBuffer(mesh->numVertices, renderData->vertexDecl) != mesh->numVertices)
				{
					LOG_WARNING("Not enough space in transient buffer");
					return;
				}

				bgfx::allocTransientIndexBuffer(&renderData->transientIndexBuffer, mesh->numIndices);
				memcpy(renderData->transientIndexBuffer.data, mesh->indices, mesh->GetSizeOfIndices());

				bgfx::allocTransientVertexBuffer(&renderData->transientVertexBuffer, mesh->numVertices, renderData->vertexDecl);
				memcpy(renderData->transientVertexBuffer.data, mesh->vertices, mesh->GetSizeOfVertices());
				break;
			}
		}

		mesh->isUpdateRequired = false;
	}
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

void RenderBgfx::Unbind(Texture *texture)
{
	ASSERT(texture != nullptr);

	if(texture->renderData == nullptr)
	{
		// Not yet bound
		return;
	}

	bgfx::TextureHandle *renderData = static_cast<bgfx::TextureHandle*>(texture->renderData);
	bgfx::destroyTexture(*renderData);

	delete renderData;
	texture->renderData = nullptr;
}

void RenderBgfx::SetTexture(uint8_t samplerIdx, Texture *texture)
{
	if(texture->renderData == nullptr)
	{
		Bind(texture);
	}

	bgfx::TextureHandle *renderData = static_cast<bgfx::TextureHandle*>(texture->renderData);
	bgfx::setTexture(samplerIdx, samplerUniforms[samplerIdx], *renderData);
}

void RenderBgfx::SetViewport(uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
	const uint8_t viewId = 0;
	bgfx::setViewRect(viewId, x, y, width, height);
}

void RenderBgfx::SetScissor(uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
	bgfx::setScissor(x, y, width, height);
}

bgfx::ProgramHandle RenderBgfx::CreateShaderProgram(const char *vertexShaderFilename, const char *fragmentShaderFilename)
{
	bgfx::ShaderHandle vsh = BGFX_INVALID_HANDLE;
	bgfx::ShaderHandle fsh = BGFX_INVALID_HANDLE;

	const char *shadersPath = nullptr;
	switch(bgfx::getRendererType())
	{
		case bgfx::RendererType::Direct3D11:	shadersPath = "data/shaders/dx11/"; break;
		case bgfx::RendererType::Direct3D12:	shadersPath = "data/shaders/dx11/"; break;
		case bgfx::RendererType::Metal:			shadersPath = "data/shaders/metal/"; break;
		case bgfx::RendererType::Vulkan:		shadersPath = "data/shaders/spirv/"; break;
		default:
			ASSERT(false && "Not supported render type");
			return bgfx::createProgram(vsh, fsh, false);
	}

	{
		String filename = String::CreateFormattedString("%s%s.bin", shadersPath, vertexShaderFilename);
		ReadFilePtr file = FileSystem::Get()->Open(filename.c_str());
		SMemoryBuffer content = file->ReadFileContent();
		vsh = bgfx::createShader(bgfx::copy(content.data, (uint32_t)content.size));
	}

	{
		String filename = String::CreateFormattedString("%s%s.bin", shadersPath, fragmentShaderFilename);
		ReadFilePtr file = FileSystem::Get()->Open(filename.c_str());
		SMemoryBuffer content = file->ReadFileContent();
		fsh = bgfx::createShader(bgfx::copy(content.data, (uint32_t)content.size));
	}

	return bgfx::createProgram(vsh, fsh, true);
}
