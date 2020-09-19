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
	pActiveWindow = &window;

	SetRenderTarget(nullptr);
	pActiveRenderTarget = nullptr;

	Clear();
}

void Render::RenderRenderable(Renderable& renderable, uint32_t indexOffset, uint32_t vertexOffset, uint32_t numIndices, uint32_t numVertices)
{
	if(renderable.mesh == nullptr)
	{
		ASSERT(false && "Invalid mesh");
		return;
	}

	if(renderable.material == nullptr || renderable.material->vertexShader == nullptr || renderable.material->fragmentShader == nullptr)
	{
		ASSERT(false && "Invalid material");
		return;
	}

	if(numIndices == UINT32_MAX)
	{
		// Use all the indices
		numIndices = renderable.mesh->GetNumIndices() - indexOffset;
	}

	if(numVertices == UINT32_MAX)
	{
		// Use all the vertices
		numVertices = renderable.mesh->GetNumVertices() - vertexOffset;
	}

	if(indexOffset + numIndices > renderable.mesh->GetNumIndices() ||
		vertexOffset + numVertices > renderable.mesh->GetNumVertices())
	{
		return;
	}

	if(renderable.mesh->pRenderData == nullptr)
	{
		Bind(renderable.mesh.get());
	}
	else if(renderable.mesh->GetType() == Mesh::EType::Dynamic)
	{
		DynamicMesh* pDynamicMesh = static_cast<DynamicMesh*>(renderable.mesh.get());
		UpdateDynamicMesh(*pDynamicMesh);
	}

	if(pActiveMesh != renderable.mesh.get())
	{
		SetMesh(*renderable.mesh);
		pActiveMesh = renderable.mesh.get();
	}

	SetMaterial(*renderable.material);
	UpdateShaderParams(*renderable.material);

	DoRender(renderable, indexOffset, vertexOffset, numIndices, numVertices);
}

void Render::SetActiveRenderTarget(RenderTarget* pRenderTarget)
{
	if(pRenderTarget != nullptr && pRenderTarget->pRenderData == nullptr)
	{
		Bind(*pRenderTarget);
	}

	if(pActiveRenderTarget != pRenderTarget)
	{
		SetRenderTarget(pRenderTarget);
		pActiveRenderTarget = pRenderTarget;
	}
}

void Render::SetBlendMode(EBlend srcBlend, EBlend destBlend, EBlendOp blendOp)
{
	SetBlendModeSeparated(srcBlend, destBlend, blendOp, srcBlend, destBlend, blendOp);
}

void Render::Bind(Mesh* pMesh)
{
	if(pMesh == nullptr || pMesh->pRenderData != nullptr)
	{
		return;
	}

	pMesh->pRenderData = CreateMeshRenderData(pMesh);
}

void Render::Bind(Shader* pShader)
{
	if(pShader == nullptr || pShader->pRenderData != nullptr)
	{
		return;
	}

	pShader->pRenderData = CreateShaderRenderData(pShader);
}

void Render::Bind(Texture* pTexture)
{
	if(pTexture == nullptr || pTexture->pRenderData != nullptr)
	{
		return;
	}

	pTexture->pRenderData = CreateTextureRenderData(pTexture);
}

void Render::Bind(RenderTarget& renderTarget)
{
	if(renderTarget.pRenderData != nullptr)
	{
		return;
	}

	const uint8_t textureCount = renderTarget.GetTextureCount();
	for(uint8_t i = 0; i < textureCount; ++i)
	{
		const std::shared_ptr<Texture>& texture = renderTarget.GetTexture(i);
		Bind(texture.get());
	}

	renderTarget.pRenderData = CreateRenderTargetRenderData(renderTarget);
}

void Render::Unbind(Mesh& mesh)
{
	if(mesh.pRenderData == nullptr)
	{
		return;
	}

	DestroyMeshRenderData(mesh.pRenderData);
	mesh.pRenderData = nullptr;

	if(pActiveMesh == &mesh)
	{
		pActiveMesh = nullptr;
	}
}

void Render::Unbind(Shader& shader)
{
	if(shader.pRenderData == nullptr)
	{
		return;
	}

	DestroyShaderRenderData(shader.pRenderData);
	shader.pRenderData = nullptr;

	if(pActiveVertexShader == &shader)
	{
		pActiveVertexShader = nullptr;
	}
	if(pActiveFragmentShader == &shader)
	{
		pActiveFragmentShader = nullptr;
	}
}

void Render::Unbind(Texture& texture)
{
	if(texture.pRenderData == nullptr)
	{
		return;
	}

	DestroyTextureRenderData(texture.pRenderData);
	texture.pRenderData = nullptr;
}

void Render::Unbind(RenderTarget& renderTarget)
{
	if(renderTarget.pRenderData == nullptr)
	{
		return;
	}

	DestroyRenderTargetRenderData(renderTarget.pRenderData);
	renderTarget.pRenderData = nullptr;

	const uint8_t textureCount = renderTarget.GetTextureCount();
	for(uint8_t i = 0; i < textureCount; ++i)
	{
		Unbind(*renderTarget.GetTexture(i));
	}

	if(pActiveRenderTarget == &renderTarget)
	{
		pActiveRenderTarget = nullptr;
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
	if(material.vertexShader != nullptr)
	{
		if(material.vertexShader->pRenderData == nullptr)
		{
			Bind(material.vertexShader.get());
		}

		if(pActiveVertexShader != material.vertexShader.get())
		{
			SetVertexShader(*material.vertexShader);
			pActiveVertexShader = material.vertexShader.get();
		}
	}

	// Fragment shader
	if(material.fragmentShader != nullptr)
	{
		if(material.fragmentShader->pRenderData == nullptr)
		{
			Bind(material.fragmentShader.get());
		}

		if(pActiveFragmentShader != material.fragmentShader.get())
		{
			SetFragmentShader(*material.fragmentShader);
			pActiveFragmentShader = material.fragmentShader.get();
		}
	}

	// Textures
	uint8_t samplerIdx = 0;
	for(const Material::TextureInfo& textureInfo : material.GetTextures())
	{
		if(textureInfo.texture->pRenderData == nullptr)
		{
			Bind(textureInfo.texture.get());
		}

		SetTexture(samplerIdx, *textureInfo.texture, textureInfo.filterMode, textureInfo.addressMode);
		samplerIdx++;
	}
}
