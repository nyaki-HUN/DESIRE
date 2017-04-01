#include "stdafx.h"
#include "Render/Render.h"
#include "Render/bgfx/MeshRenderDataBgfx.h"
#include "Core/IWindow.h"
#include "Core/String.h"
#include "Resource/Mesh.h"
#include "Resource/Texture.h"

#include "bgfx/bgfx.h"
#include "bgfx/platform.h"

Render::Render()
	: initialized(false)
{

}

Render::~Render()
{

}

void Render::Init(IWindow *mainWindow)
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

void Render::Kill()
{
	bgfx::shutdown();
	initialized = false;
}

void Render::UpdateRenderWindow(IWindow *window)
{
	if(!initialized)
	{
		return;
	}

	bgfx::reset(window->GetWidth(), window->GetHeight(), BGFX_RESET_VSYNC);
}

void Render::BeginFrame(IWindow *window)
{
	bgfx::setViewRect(0, 0, 0, window->GetWidth(), window->GetHeight());

	// This dummy draw call is here to make sure that view 0 is cleared if no other draw calls are submitted to view 0
	bgfx::touch(0);
}

void Render::EndFrame()
{
	bgfx::frame();
}

void Render::Bind(Mesh *mesh)
{
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

	const bgfx::Memory *indexData = bgfx::makeRef(mesh->indices.data(), (uint32_t)mesh->indices.size());
	const bgfx::Memory *vertexData = bgfx::makeRef(mesh->vertices.data(), (uint32_t)mesh->vertices.size());

	renderData->indexBuffer = bgfx::createIndexBuffer(indexData, BGFX_BUFFER_NONE);
	renderData->vertexBuffer = bgfx::createVertexBuffer(vertexData, decl, BGFX_BUFFER_NONE);
}

void Render::UnBind(Mesh *mesh)
{
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

void Render::Bind(Texture *texture)
{
	if(texture->renderData != nullptr)
	{
		// Already bound
		return;
	}

	bgfx::TextureFormat::Enum format = bgfx::TextureFormat::Unknown;
	switch(texture->format)
	{
		case Texture::EFormat::UNKNOWN:		format = bgfx::TextureFormat::Unknown; break;
		case Texture::EFormat::RGB8:		format = bgfx::TextureFormat::RGB8; break;
		case Texture::EFormat::RGBA8:		format = bgfx::TextureFormat::RGBA8; break;
	}

	bgfx::TextureHandle handle = bgfx::createTexture2D(texture->width, texture->height, (texture->numMipMaps != 0), 0, format, BGFX_TEXTURE_NONE, bgfx::makeRef(texture->data.data, (uint32_t)texture->data.size));
	if(bgfx::isValid(handle))
	{
		texture->renderData = new bgfx::TextureHandle(handle);
	}
}

void Render::Unbind(Texture *texture)
{
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

bool Render::IsUsingOpenGL()
{
	const bool isOpenGL =
		bgfx::getRendererType() == bgfx::RendererType::OpenGLES ||
		bgfx::getRendererType() == bgfx::RendererType::OpenGL;

	return isOpenGL;
}

bgfx::ProgramHandle CreateShaderProgram(const char *vertexShaderFilename, const char *fragmentShaderFilename)
{
	const char *shadersPath = nullptr;
	switch(bgfx::getRendererType())
	{
		case bgfx::RendererType::Noop:			break;
		case bgfx::RendererType::Direct3D9:		shadersPath = "data/shaders/dx9/"; break;
		case bgfx::RendererType::Direct3D11:	shadersPath = "data/shaders/dx11/"; break;
		case bgfx::RendererType::Direct3D12:	shadersPath = "data/shaders/dx11/"; break;
		case bgfx::RendererType::Gnm:			shadersPath = "data/shaders/pssl/"; break;
		case bgfx::RendererType::Metal:			shadersPath = "data/shaders/metal/"; break;
		case bgfx::RendererType::OpenGLES:		shadersPath = "data/shaders/essl/"; break;
		case bgfx::RendererType::OpenGL:		shadersPath = "data/shaders/glsl/"; break;
		case bgfx::RendererType::Vulkan:		shadersPath = "data/shaders/spirv/"; break;
		case bgfx::RendererType::Count:			ASSERT(false); break;
	}

	if(shadersPath == nullptr)
	{
		ASSERT(false && "Unhandled render type");
		return bgfx::createProgram(BGFX_INVALID_HANDLE, BGFX_INVALID_HANDLE, false);
	}

	String filename = String::CreateFormattedString("%s%s.bin", shadersPath, vertexShaderFilename);
	bgfx::Memory *content = nullptr;
	bgfx::ShaderHandle vsh = bgfx::createShader(content);
	filename = String::CreateFormattedString("%s%s.bin", shadersPath, fragmentShaderFilename);
	bgfx::ShaderHandle fsh = bgfx::createShader(content);
	return bgfx::createProgram(vsh, fsh, true);
}
