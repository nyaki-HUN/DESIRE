#include "Engine/stdafx.h"
#include "Engine/Render/Render.h"

#include "Engine/Application/OSWindow.h"

#include "Engine/Render/Material.h"
#include "Engine/Render/Mesh.h"
#include "Engine/Render/Renderable.h"
#include "Engine/Render/RenderData.h"
#include "Engine/Render/RenderTarget.h"
#include "Engine/Render/Shader.h"
#include "Engine/Render/Texture.h"

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
	if(renderable.m_spMesh == nullptr)
	{
		ASSERT(false && "Invalid mesh");
		return;
	}

	if(renderable.m_spMaterial == nullptr || renderable.m_spMaterial->m_spVertexShader == nullptr || renderable.m_spMaterial->m_spPixelShader == nullptr)
	{
		ASSERT(false && "Invalid material");
		return;
	}

	if(numIndices == UINT32_MAX)
	{
		// Use all the indices
		numIndices = renderable.m_spMesh->GetNumIndices() - indexOffset;
	}

	if(numVertices == UINT32_MAX)
	{
		// Use all the vertices
		numVertices = renderable.m_spMesh->GetNumVertices() - vertexOffset;
	}

	if(indexOffset + numIndices > renderable.m_spMesh->GetNumIndices() ||
		vertexOffset + numVertices > renderable.m_spMesh->GetNumVertices())
	{
		return;
	}

	// If the mesh or the shaders changed we need to rebind the renderable
	if(renderable.m_spMesh->m_pRenderData == nullptr ||
		renderable.m_spMaterial->m_spVertexShader->m_pRenderData == nullptr ||
		renderable.m_spMaterial->m_spPixelShader->m_pRenderData == nullptr)
	{
		Unbind(renderable);
	}

	// Mesh
	if(renderable.m_spMesh->m_pRenderData == nullptr)
	{
		renderable.m_spMesh->m_pRenderData = CreateMeshRenderData(*renderable.m_spMesh);
	}
	else if(renderable.m_spMesh->GetType() == Mesh::EType::Dynamic)
	{
		DynamicMesh* pDynamicMesh = static_cast<DynamicMesh*>(renderable.m_spMesh.get());
		UpdateDynamicMesh(*pDynamicMesh);
	}

	if(m_pActiveMesh != renderable.m_spMesh.get())
	{
		SetMesh(*renderable.m_spMesh);
		m_pActiveMesh = renderable.m_spMesh.get();
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
	if(pRenderTarget != nullptr && pRenderTarget->m_pRenderData == nullptr)
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

void Render::Unbind(Mesh& mesh)
{
	if(mesh.m_pRenderData == nullptr)
	{
		return;
	}

	if(m_pActiveMesh == &mesh)
	{
		m_pActiveMesh = nullptr;
	}

	OnDestroyMeshRenderData(mesh.m_pRenderData);
	delete mesh.m_pRenderData;
	mesh.m_pRenderData = nullptr;
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
