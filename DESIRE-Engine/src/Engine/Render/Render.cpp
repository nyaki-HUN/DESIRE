#include "Engine/stdafx.h"
#include "Engine/Render/Render.h"

#include "Engine/Render/Material.h"

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
		vertexOffset + vertexOffset > pMesh->numVertices)
	{
		return;
	}

	if(pMesh->pRenderData == nullptr)
	{
		Bind(pMesh);
	}
	else if(pMesh->type == Mesh::EType::Dynamic)
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
	ASSERT(screenSpaceQuadVertexShader->renderData != nullptr && "Shader needs to be bound by the render module");

	if(pMaterial == nullptr || pMaterial->vertexShader == nullptr || pMaterial->fragmentShader == nullptr)
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
		if(pMaterial->vertexShader->renderData == nullptr)
		{
			Bind(pMaterial->vertexShader.get());
		}

		SetVertexShader(pMaterial->vertexShader.get());
	}

	// Fragment shader
	if(pMaterial->fragmentShader != nullptr)
	{
		if(pMaterial->fragmentShader->renderData == nullptr)
		{
			Bind(pMaterial->fragmentShader.get());
		}

		SetFragmentShader(pMaterial->fragmentShader.get());
	}

	// Textures
	uint8_t samplerIdx = 0;
	for(const Material::TextureInfo& textureInfo : pMaterial->GetTextures())
	{
		if(textureInfo.texture->renderData == nullptr)
		{
			Bind(textureInfo.texture.get());
		}

		SetTexture(samplerIdx, textureInfo.texture.get(), textureInfo.filterMode, textureInfo.addressMode);
		samplerIdx++;
	}
}
