#include "Engine/stdafx.h"
#include "Engine/Render/Render.h"

#include "Engine/Application/OSWindow.h"

#include "Engine/Render/IndexBuffer.h"
#include "Engine/Render/Material.h"
#include "Engine/Render/Renderable.h"
#include "Engine/Render/RenderData.h"
#include "Engine/Render/RenderTarget.h"
#include "Engine/Render/Shader.h"
#include "Engine/Render/Texture.h"
#include "Engine/Render/VertexBuffer.h"

Render::Render()
{
}

Render::~Render()
{
}

void Render::BeginFrame(OSWindow& window)
{
	m_pActiveWindow = &window;

	SetRenderTarget(nullptr);
	m_pActiveRenderTarget = nullptr;

	Clear();
}

void Render::RenderRenderable(Renderable& renderable, uint32_t indexOffset, uint32_t vertexOffset, uint32_t numIndices, uint32_t numVertices)
{
	if((renderable.m_spIndexBuffer && indexOffset >= renderable.m_spIndexBuffer->GetNumIndices()) ||
		renderable.m_spVertexBuffer == nullptr ||
		vertexOffset >= renderable.m_spVertexBuffer->GetNumVertices())
	{
		// Nothing to render
		return;
	}

	if(renderable.m_spMaterial == nullptr || renderable.m_spMaterial->m_spVertexShader == nullptr || renderable.m_spMaterial->m_spPixelShader == nullptr)
	{
		ASSERT(false && "Invalid material");
		return;
	}

	// If the mesh or the shaders changed we need to rebind the renderable
	if(renderable.m_spVertexBuffer->m_pRenderData == nullptr ||
		renderable.m_spMaterial->m_spVertexShader->m_pRenderData == nullptr ||
		renderable.m_spMaterial->m_spPixelShader->m_pRenderData == nullptr)
	{
		Unbind(renderable);
	}

	// Index buffer
	if(renderable.m_spIndexBuffer)
	{
		if(numIndices > renderable.m_spIndexBuffer->GetNumIndices())
		{
			// Use all the available indices
			numIndices = renderable.m_spIndexBuffer->GetNumIndices() - indexOffset;
		}

		if(numIndices == 0)
		{
			// Nothing to render
			return;
		}

		if(renderable.m_spIndexBuffer->m_pRenderData == nullptr)
		{
			renderable.m_spIndexBuffer->m_pRenderData = CreateIndexBufferRenderData(*renderable.m_spIndexBuffer);
		}
		else if(renderable.m_spIndexBuffer->GetFlags() & DeviceBuffer::DIRTY)
		{
			UpdateDeviceBuffer(*renderable.m_spIndexBuffer);
		}

		if(m_pActiveIndexBuffer != renderable.m_spIndexBuffer.get())
		{
			SetIndexBuffer(*renderable.m_spIndexBuffer);
			m_pActiveIndexBuffer = renderable.m_spIndexBuffer.get();
		}
	}

	// Vertex buffer
	if(numVertices > renderable.m_spVertexBuffer->GetNumVertices())
	{
		// Use all the available vertices
		numVertices = renderable.m_spVertexBuffer->GetNumVertices() - vertexOffset;
	}

	if(numVertices == 0)
	{
		// Nothing to render
		return;
	}

	if(renderable.m_spVertexBuffer->m_pRenderData == nullptr)
	{
		renderable.m_spVertexBuffer->m_pRenderData = CreateVertexBufferRenderData(*renderable.m_spVertexBuffer);
	}
	else if(renderable.m_spVertexBuffer->GetFlags() & DeviceBuffer::DIRTY)
	{
		UpdateDeviceBuffer(*renderable.m_spVertexBuffer);
	}

	if(numVertices == UINT32_MAX)
	{
		// Use all the vertices
		numVertices = renderable.m_spVertexBuffer->GetNumVertices() - vertexOffset;
	}

	if(m_pActiveVertexBuffer != renderable.m_spVertexBuffer.get())
	{
		SetVertexBuffer(*renderable.m_spVertexBuffer);
		m_pActiveVertexBuffer = renderable.m_spVertexBuffer.get();
	}

	// Vertex shader
	if(renderable.m_spMaterial->m_spVertexShader->m_pRenderData == nullptr)
	{
		renderable.m_spMaterial->m_spVertexShader->m_pRenderData = CreateShaderRenderData(*renderable.m_spMaterial->m_spVertexShader);
	}

	// Pixel shader
	if(renderable.m_spMaterial->m_spPixelShader->m_pRenderData == nullptr)
	{
		renderable.m_spMaterial->m_spPixelShader->m_pRenderData = CreateShaderRenderData(*renderable.m_spMaterial->m_spPixelShader);
	}

	// Textures
	for(const Material::TextureInfo& textureInfo : renderable.m_spMaterial->GetTextures())
	{
		if(textureInfo.spTexture->m_pRenderData == nullptr)
		{
			textureInfo.spTexture->m_pRenderData = CreateTextureRenderData(*textureInfo.spTexture);
		}
	}

	if(renderable.m_pRenderData == nullptr)
	{
		renderable.m_pRenderData = CreateRenderableRenderData(renderable);
	}

	UpdateShaderParams(*renderable.m_spMaterial);

	DoRender(renderable, indexOffset, vertexOffset, numIndices, numVertices);
}

void Render::SetActiveRenderTarget(RenderTarget* pRenderTarget)
{
	if(pRenderTarget && pRenderTarget->m_pRenderData == nullptr)
	{
		const uint8_t textureCount = pRenderTarget->GetTextureCount();
		for(uint8_t i = 0; i < textureCount; ++i)
		{
			const std::shared_ptr<Texture>& spTexture = pRenderTarget->GetTexture(i);
			if(spTexture->m_pRenderData == nullptr)
			{
				spTexture->m_pRenderData = CreateTextureRenderData(*spTexture);
			}
		}

		pRenderTarget->m_pRenderData = CreateRenderTargetRenderData(*pRenderTarget);
	}

	if(m_pActiveRenderTarget != pRenderTarget)
	{
		SetRenderTarget(pRenderTarget);
		m_pActiveRenderTarget = pRenderTarget;
	}
}

void Render::Unbind(Renderable& renderable)
{
	if(renderable.m_pRenderData == nullptr)
	{
		return;
	}

	OnDestroyRenderableRenderData(renderable.m_pRenderData);
	delete renderable.m_pRenderData;
	renderable.m_pRenderData = nullptr;
}

void Render::Unbind(IndexBuffer& indexBuffer)
{
	if(indexBuffer.m_pRenderData == nullptr)
	{
		return;
	}

	if(m_pActiveIndexBuffer == &indexBuffer)
	{
		m_pActiveIndexBuffer = nullptr;
	}

	delete indexBuffer.m_pRenderData;
	indexBuffer.m_pRenderData = nullptr;
}

void Render::Unbind(VertexBuffer& vertexBuffer)
{
	if(vertexBuffer.m_pRenderData == nullptr)
	{
		return;
	}

	if(m_pActiveVertexBuffer == &vertexBuffer)
	{
		m_pActiveVertexBuffer = nullptr;
	}

	delete vertexBuffer.m_pRenderData;
	vertexBuffer.m_pRenderData = nullptr;
}

void Render::Unbind(Shader& shader)
{
	if(shader.m_pRenderData == nullptr)
	{
		return;
	}

	OnDestroyShaderRenderData(shader.m_pRenderData);
	delete shader.m_pRenderData;
	shader.m_pRenderData = nullptr;
}

void Render::Unbind(Texture& texture)
{
	if(texture.m_pRenderData == nullptr)
	{
		return;
	}

	OnDestroyTextureRenderData(texture.m_pRenderData);
	delete texture.m_pRenderData;
	texture.m_pRenderData = nullptr;
}

void Render::Unbind(RenderTarget& renderTarget)
{
	if(renderTarget.m_pRenderData == nullptr)
	{
		return;
	}

	if(m_pActiveRenderTarget == &renderTarget)
	{
		m_pActiveRenderTarget = nullptr;
	}

	const uint8_t textureCount = renderTarget.GetTextureCount();
	for(uint8_t i = 0; i < textureCount; ++i)
	{
		Unbind(*renderTarget.GetTexture(i));
	}

	OnDestroyRenderTargetRenderData(renderTarget.m_pRenderData);
	delete renderTarget.m_pRenderData;
	renderTarget.m_pRenderData = nullptr;
}
