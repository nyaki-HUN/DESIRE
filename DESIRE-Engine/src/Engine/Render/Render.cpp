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
	if(renderable.m_mesh == nullptr)
	{
		ASSERT(false && "Invalid mesh");
		return;
	}

	if(renderable.m_material == nullptr || renderable.m_material->m_vertexShader == nullptr || renderable.m_material->m_pixelShader == nullptr)
	{
		ASSERT(false && "Invalid material");
		return;
	}

	if(numIndices == UINT32_MAX)
	{
		// Use all the indices
		numIndices = renderable.m_mesh->GetNumIndices() - indexOffset;
	}

	if(numVertices == UINT32_MAX)
	{
		// Use all the vertices
		numVertices = renderable.m_mesh->GetNumVertices() - vertexOffset;
	}

	if(indexOffset + numIndices > renderable.m_mesh->GetNumIndices() ||
		vertexOffset + numVertices > renderable.m_mesh->GetNumVertices())
	{
		return;
	}

	// If the mesh or the shaders changed we need to rebind the renderable
	if(renderable.m_mesh->m_pRenderData == nullptr ||
		renderable.m_material->m_vertexShader->m_pRenderData == nullptr ||
		renderable.m_material->m_pixelShader->m_pRenderData == nullptr)
	{
		Unbind(renderable);
	}

	// Mesh
	if(renderable.m_mesh->m_pRenderData == nullptr)
	{
		renderable.m_mesh->m_pRenderData = CreateMeshRenderData(*renderable.m_mesh);
	}
	else if(renderable.m_mesh->GetType() == Mesh::EType::Dynamic)
	{
		DynamicMesh* pDynamicMesh = static_cast<DynamicMesh*>(renderable.m_mesh.get());
		UpdateDynamicMesh(*pDynamicMesh);
	}

	if(m_pActiveMesh != renderable.m_mesh.get())
	{
		SetMesh(*renderable.m_mesh);
		m_pActiveMesh = renderable.m_mesh.get();
	}

	// Vertex shader
	if(renderable.m_material->m_vertexShader->m_pRenderData == nullptr)
	{
		renderable.m_material->m_vertexShader->m_pRenderData = CreateShaderRenderData(*renderable.m_material->m_vertexShader);
	}

	// Pixel shader
	if(renderable.m_material->m_pixelShader->m_pRenderData == nullptr)
	{
		renderable.m_material->m_pixelShader->m_pRenderData = CreateShaderRenderData(*renderable.m_material->m_pixelShader);
	}

	// Textures
	for(const Material::TextureInfo& textureInfo : renderable.m_material->GetTextures())
	{
		if(textureInfo.m_texture->m_pRenderData == nullptr)
		{
			textureInfo.m_texture->m_pRenderData = CreateTextureRenderData(*textureInfo.m_texture);
		}
	}

	if(renderable.m_pRenderData == nullptr)
	{
		renderable.m_pRenderData = CreateRenderableRenderData(renderable);
	}

	UpdateShaderParams(*renderable.m_material);

	DoRender(renderable, indexOffset, vertexOffset, numIndices, numVertices);
}

void Render::SetActiveRenderTarget(RenderTarget* pRenderTarget)
{
	if(pRenderTarget != nullptr && pRenderTarget->m_pRenderData == nullptr)
	{
		const uint8_t textureCount = pRenderTarget->GetTextureCount();
		for(uint8_t i = 0; i < textureCount; ++i)
		{
			const std::shared_ptr<Texture>& texture = pRenderTarget->GetTexture(i);
			if(texture->m_pRenderData == nullptr)
			{
				texture->m_pRenderData = CreateTextureRenderData(*texture);
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
