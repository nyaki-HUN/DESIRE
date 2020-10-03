#include "Engine/stdafx.h"
#include "Engine/Render/Render.h"

#include "Engine/Application/OSWindow.h"

#include "Engine/Render/Material.h"
#include "Engine/Render/Mesh.h"
#include "Engine/Render/Renderable.h"
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

	if(renderable.m_material == nullptr || renderable.m_material->m_vertexShader == nullptr || renderable.m_material->m_fragmentShader == nullptr)
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

	if(renderable.m_mesh->m_pRenderData == nullptr)
	{
		Bind(*renderable.m_mesh);
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

	SetMaterial(*renderable.m_material);
	UpdateShaderParams(*renderable.m_material);

	DoRender(renderable, indexOffset, vertexOffset, numIndices, numVertices);
}

void Render::SetActiveRenderTarget(RenderTarget* pRenderTarget)
{
	if(pRenderTarget != nullptr && pRenderTarget->m_pRenderData == nullptr)
	{
		Bind(*pRenderTarget);
	}

	if(m_pActiveRenderTarget != pRenderTarget)
	{
		SetRenderTarget(pRenderTarget);
		m_pActiveRenderTarget = pRenderTarget;
	}
}

void Render::SetBlendMode(EBlend srcBlend, EBlend destBlend, EBlendOp blendOp)
{
	SetBlendModeSeparated(srcBlend, destBlend, blendOp, srcBlend, destBlend, blendOp);
}

void Render::Bind(Renderable& renderable)
{
	if(renderable.m_pRenderData == nullptr)
	{
		renderable.m_pRenderData = CreateRenderableRenderData(renderable);
	}
}

void Render::Bind(Mesh& mesh)
{
	if(mesh.m_pRenderData == nullptr)
	{
		mesh.m_pRenderData = CreateMeshRenderData(mesh);
	}
}

void Render::Bind(Shader& shader)
{
	if(shader.m_pRenderData == nullptr)
	{
		shader.m_pRenderData = CreateShaderRenderData(shader);
	}
}

void Render::Bind(Texture& texture)
{
	if(texture.m_pRenderData == nullptr)
	{
		texture.m_pRenderData = CreateTextureRenderData(texture);
	}
}

void Render::Bind(RenderTarget& renderTarget)
{
	if(renderTarget.m_pRenderData != nullptr)
	{
		return;
	}

	const uint8_t textureCount = renderTarget.GetTextureCount();
	for(uint8_t i = 0; i < textureCount; ++i)
	{
		const std::shared_ptr<Texture>& texture = renderTarget.GetTexture(i);
		Bind(*texture);
	}

	renderTarget.m_pRenderData = CreateRenderTargetRenderData(renderTarget);
}

void Render::Unbind(Renderable& renderable)
{
	if(renderable.m_pRenderData == nullptr)
	{
		return;
	}

	if( renderable.m_mesh == nullptr ||
		renderable.m_material == nullptr ||
		renderable.m_material->m_vertexShader == nullptr ||
		renderable.m_material->m_fragmentShader == nullptr)
	{
		ASSERT(false && "Invalid renderable");
		return;
	}

	Bind(*renderable.m_mesh);
	Bind(*renderable.m_material->m_vertexShader);
	Bind(*renderable.m_material->m_fragmentShader);

	DestroyRenderableRenderData(renderable.m_pRenderData);
	renderable.m_pRenderData = nullptr;
}

void Render::Unbind(Mesh& mesh)
{
	if(mesh.m_pRenderData == nullptr)
	{
		return;
	}

	DestroyMeshRenderData(mesh.m_pRenderData);
	mesh.m_pRenderData = nullptr;

	if(m_pActiveMesh == &mesh)
	{
		m_pActiveMesh = nullptr;
	}
}

void Render::Unbind(Shader& shader)
{
	if(shader.m_pRenderData == nullptr)
	{
		return;
	}

	DestroyShaderRenderData(shader.m_pRenderData);
	shader.m_pRenderData = nullptr;

	if(m_pActiveVertexShader == &shader)
	{
		m_pActiveVertexShader = nullptr;
	}
	if(m_pActiveFragmentShader == &shader)
	{
		m_pActiveFragmentShader = nullptr;
	}
}

void Render::Unbind(Texture& texture)
{
	if(texture.m_pRenderData == nullptr)
	{
		return;
	}

	DestroyTextureRenderData(texture.m_pRenderData);
	texture.m_pRenderData = nullptr;
}

void Render::Unbind(RenderTarget& renderTarget)
{
	if(renderTarget.m_pRenderData == nullptr)
	{
		return;
	}

	DestroyRenderTargetRenderData(renderTarget.m_pRenderData);
	renderTarget.m_pRenderData = nullptr;

	const uint8_t textureCount = renderTarget.GetTextureCount();
	for(uint8_t i = 0; i < textureCount; ++i)
	{
		Unbind(*renderTarget.GetTexture(i));
	}

	if(m_pActiveRenderTarget == &renderTarget)
	{
		m_pActiveRenderTarget = nullptr;
	}
}

void Render::SetDefaultRenderStates()
{
	SetColorWriteEnabled(true, true, true, true);
	SetDepthWriteEnabled(true);
	SetDepthTest(EDepthTest::Less);
	SetCullMode(ECullMode::CCW);
}

void Render::SetMaterial(Material& material)
{
	// Vertex shader
	if(material.m_vertexShader != nullptr)
	{
		if(material.m_vertexShader->m_pRenderData == nullptr)
		{
			Bind(*material.m_vertexShader);
		}

		if(m_pActiveVertexShader != material.m_vertexShader.get())
		{
			SetVertexShader(*material.m_vertexShader);
			m_pActiveVertexShader = material.m_vertexShader.get();
		}
	}

	// Fragment shader
	if(material.m_fragmentShader != nullptr)
	{
		if(material.m_fragmentShader->m_pRenderData == nullptr)
		{
			Bind(*material.m_fragmentShader);
		}

		if(m_pActiveFragmentShader != material.m_fragmentShader.get())
		{
			SetFragmentShader(*material.m_fragmentShader);
			m_pActiveFragmentShader = material.m_fragmentShader.get();
		}
	}

	// Textures
	uint8_t samplerIdx = 0;
	for(const Material::TextureInfo& textureInfo : material.GetTextures())
	{
		if(textureInfo.m_texture->m_pRenderData == nullptr)
		{
			Bind(*textureInfo.m_texture);
		}

		SetTexture(samplerIdx, *textureInfo.m_texture, textureInfo.m_filterMode, textureInfo.m_addressMode);
		samplerIdx++;
	}
}
