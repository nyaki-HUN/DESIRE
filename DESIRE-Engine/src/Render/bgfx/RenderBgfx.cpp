#include "stdafx.h"
#include "Render/bgfx/RenderBgfx.h"
#include "Render/bgfx/MeshRenderDataBgfx.h"
#include "Core/IWindow.h"
#include "Core/String.h"
#include "Core/fs/FileSystem.h"
#include "Core/fs/IReadFile.h"
#include "Resource/Mesh.h"
#include "Resource/Texture.h"

#include "bgfx/bgfx.h"
#include "bgfx/platform.h"

RenderBgfx::RenderBgfx()
	: initialized(false)
{

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

	initialized = bgfx::init(bgfx::RendererType::Count);
	bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x303030ff, 1.0f, 0);
	bgfx::reset(mainWindow->GetWidth(), mainWindow->GetHeight(), BGFX_RESET_VSYNC);
}

void RenderBgfx::Kill()
{
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

	bgfx::VertexDecl decl;
	decl.begin()
		.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
		.add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
		.end();

	MeshRenderDataBgfx *renderData = new MeshRenderDataBgfx();

	const bgfx::Memory *indexData = bgfx::makeRef(mesh->indices, (uint32_t)mesh->sizeOfIndices);
	const bgfx::Memory *vertexData = bgfx::makeRef(mesh->vertices, (uint32_t)mesh->sizeOfVertices);

	renderData->indexBuffer = bgfx::createIndexBuffer(indexData, BGFX_BUFFER_NONE);
	renderData->vertexBuffer = bgfx::createVertexBuffer(vertexData, decl, BGFX_BUFFER_NONE);
}

void RenderBgfx::UnBind(Mesh *mesh)
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

	bgfx::TextureHandle handle = bgfx::createTexture2D(texture->width, texture->height, (texture->numMipMaps != 0), 0, format, BGFX_TEXTURE_NONE, bgfx::makeRef(texture->data.data, (uint32_t)texture->data.size));
	if(bgfx::isValid(handle))
	{
		texture->renderData = new bgfx::TextureHandle(handle);
	}
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

bgfx::ProgramHandle CreateShaderProgram(const char *vertexShaderFilename, const char *fragmentShaderFilename)
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
