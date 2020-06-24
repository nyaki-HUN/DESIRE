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

void Render::RenderMesh(Mesh* pMesh, Material* pMaterial)
{
	if(pMaterial == nullptr || pMaterial->vertexShader == nullptr || pMaterial->fragmentShader == nullptr)
	{
		ASSERT(false && "Invalid material");
		return;
	}

	if(pMesh != nullptr)
	{
		if(pMesh->pRenderData == nullptr)
		{
			Bind(pMesh);
		}
		else if(pMesh->type == Mesh::EType::Dynamic)
		{
			DynamicMesh* pDynamicMesh = static_cast<DynamicMesh*>(pMesh);
			UpdateDynamicMesh(*pDynamicMesh);
		}
	}

	SetMesh(pMesh);
	SetMaterial(pMaterial);
	UpdateShaderParams(pMaterial);

	DoRender();
}

void Render::RenderScreenSpaceQuad(Material* pMaterial)
{
	ASSERT(screenSpaceQuadVertexShader->renderData != nullptr && "Shader needs to be bound by the render module");

	if(pMaterial == nullptr || pMaterial->vertexShader == nullptr || pMaterial->fragmentShader == nullptr)
	{
		ASSERT(false && "Invalid material");
		return;
	}

	SetMesh(nullptr);
	SetMaterial(pMaterial);
	SetVertexShader(screenSpaceQuadVertexShader.get());
	UpdateShaderParams(pMaterial);

	DoRender();
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
