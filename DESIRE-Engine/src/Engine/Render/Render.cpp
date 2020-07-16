#include "Engine/stdafx.h"
#include "Engine/Render/Render.h"

#include "Engine/Render/Material.h"
#include "Engine/Render/RenderTarget.h"

#include "Engine/Resource/Mesh.h"
#include "Engine/Resource/Shader.h"
#include "Engine/Resource/Texture.h"

Render::Render()
{
	screenSpaceQuadVertexShader = std::make_unique<Shader>("vs_screenSpaceQuad");
}

Render::~Render()
{
}

void Render::RenderMesh(Mesh* pMesh, Material* pMaterial, uint32_t indexOffset, uint32_t vertexOffset, uint32_t numIndices, uint32_t numVertices)
{
	if(pMesh == nullptr)
	{
		ASSERT(false && "Invalid mesh");
		return;
	}

	if(pMaterial == nullptr || pMaterial->vertexShader == nullptr || pMaterial->fragmentShader == nullptr)
	{
		ASSERT(false && "Invalid material");
		return;
	}

	if(numIndices == UINT32_MAX)
	{
		// Use all the indices
		numIndices = pMesh->numIndices - indexOffset;
	}

	if(numVertices == UINT32_MAX)
	{
		// Use all the vertices
		numVertices = pMesh->numVertices - vertexOffset;
	}

	if(indexOffset + numIndices > pMesh->numIndices ||
		vertexOffset + numVertices > pMesh->numVertices)
	{
		return;
	}

	if(pMesh->pRenderData == nullptr)
	{
		Bind(pMesh);
	}
	else if(pMesh->GetType() == Mesh::EType::Dynamic)
	{
		DynamicMesh* pDynamicMesh = static_cast<DynamicMesh*>(pMesh);
		UpdateDynamicMesh(*pDynamicMesh);
	}

	if(pActiveMesh != pMesh)
	{
		SetMesh(pMesh);
		pActiveMesh = pMesh;
	}

	SetMaterial(pMaterial);
	UpdateShaderParams(pMaterial);

	DoRender(indexOffset, vertexOffset, numIndices, numVertices);
}

void Render::RenderScreenSpaceQuad(Material* pMaterial)
{
	ASSERT(screenSpaceQuadVertexShader->pRenderData != nullptr && "Shader needs to be bound by the render module");

	if(pMaterial == nullptr || pMaterial->fragmentShader == nullptr)
	{
		ASSERT(false && "Invalid material");
		return;
	}

	if(pActiveMesh != nullptr)
	{
		SetMesh(nullptr);
		pActiveMesh = nullptr;
	}

	SetMaterial(pMaterial);
	SetVertexShader(screenSpaceQuadVertexShader.get());
	UpdateShaderParams(pMaterial);

	DoRender(0, 0, 0, 4);
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

void Render::Bind(RenderTarget* pRenderTarget)
{
	if(pRenderTarget == nullptr || pRenderTarget->pRenderData != nullptr)
	{
		return;
	}

	const uint8_t textureCount = pRenderTarget->GetTextureCount();
	for(uint8_t i = 0; i < textureCount; ++i)
	{
		const std::shared_ptr<Texture>& texture = pRenderTarget->GetTexture(i);
		Bind(texture.get());
	}

	pRenderTarget->pRenderData = CreateRenderTargetRenderData(pRenderTarget);
}

void Render::Unbind(Mesh* pMesh)
{
	if(pMesh == nullptr || pMesh->pRenderData == nullptr)
	{
		return;
	}

	DestroyMeshRenderData(pMesh->pRenderData);
	pMesh->pRenderData = nullptr;

	if(pActiveMesh == pMesh)
	{
		pActiveMesh = nullptr;
	}
}

void Render::Unbind(Shader* pShader)
{
	if(pShader == nullptr || pShader->pRenderData == nullptr)
	{
		return;
	}

	DestroyShaderRenderData(pShader->pRenderData);
	pShader->pRenderData = nullptr;

	if(pActiveVertexShader == pShader)
	{
		pActiveVertexShader = nullptr;
	}
	if(pActiveFragmentShader == pShader)
	{
		pActiveFragmentShader = nullptr;
	}
}

void Render::Unbind(Texture* pTexture)
{
	if(pTexture == nullptr || pTexture->pRenderData == nullptr)
	{
		return;
	}

	DestroyTextureRenderData(pTexture->pRenderData);
	pTexture->pRenderData = nullptr;
}

void Render::Unbind(RenderTarget* pRenderTarget)
{
	if(pRenderTarget == nullptr || pRenderTarget->pRenderData == nullptr)
	{
		return;
	}

	DestroyRenderTargetRenderData(pRenderTarget->pRenderData);
	pRenderTarget->pRenderData = nullptr;

	const uint8_t textureCount = pRenderTarget->GetTextureCount();
	for(uint8_t i = 0; i < textureCount; ++i)
	{
		Unbind(pRenderTarget->GetTexture(i).get());
	}
}

void Render::SetDefaultRenderStates()
{
	SetColorWriteEnabled(true, true, true, true);
	SetDepthWriteEnabled(true);
	SetDepthTest(EDepthTest::Less);
	SetCullMode(ECullMode::CCW);
}

void Render::SetMaterial(Material* pMaterial)
{
	// Vertex shader
	if(pMaterial->vertexShader != nullptr)
	{
		if(pMaterial->vertexShader->pRenderData == nullptr)
		{
			Bind(pMaterial->vertexShader.get());
		}

		SetVertexShader(pMaterial->vertexShader.get());
	}

	// Fragment shader
	if(pMaterial->fragmentShader != nullptr)
	{
		if(pMaterial->fragmentShader->pRenderData == nullptr)
		{
			Bind(pMaterial->fragmentShader.get());
		}

		SetFragmentShader(pMaterial->fragmentShader.get());
	}

	// Textures
	uint8_t samplerIdx = 0;
	for(const Material::TextureInfo& textureInfo : pMaterial->GetTextures())
	{
		if(textureInfo.texture->pRenderData == nullptr)
		{
			Bind(textureInfo.texture.get());
		}

		SetTexture(samplerIdx, *textureInfo.texture, textureInfo.filterMode, textureInfo.addressMode);
		samplerIdx++;
	}
}
