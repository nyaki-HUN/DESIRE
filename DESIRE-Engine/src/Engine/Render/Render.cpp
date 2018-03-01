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

void Render::RenderMesh(Mesh *mesh, Material *material)
{
	ASSERT(mesh != nullptr);
	ASSERT(material != nullptr);

	if(material->vertexShader == nullptr || material->fragmentShader == nullptr)
	{
		ASSERT(false && "Invalid material");
		return;
	}

	if(mesh->renderData == nullptr)
	{
		Bind(mesh);
	}
	else if(mesh->type == Mesh::EType::DYNAMIC)
	{
		UpdateDynamicMesh(static_cast<DynamicMesh*>(mesh));
	}

	SetMesh(mesh);
	SetMaterial(material);
	UpdateShaderParams(material);

	DoRender();
}

void Render::RenderScreenSpaceQuad(Material *material)
{
	ASSERT(material != nullptr);
	ASSERT(screenSpaceQuadVertexShader->renderData != nullptr && "Shader needs to be bound by the render module");

	if(material->vertexShader != nullptr || material->fragmentShader == nullptr)
	{
		ASSERT(false && "Invalid material");
		return;
	}

	SetScreenSpaceQuadMesh();
	SetMaterial(material);
	SetVertexShader(screenSpaceQuadVertexShader.get());
	UpdateShaderParams(material);

	DoRender();
}

void Render::SetBlendMode(EBlend srcBlend, EBlend destBlend, EBlendOp blendOp)
{
	SetBlendModeSeparated(srcBlend, destBlend, blendOp, srcBlend, destBlend, blendOp);
}

void Render::SetDefaultRenderStates()
{
	SetColorWriteEnabled(true, true);
	SetDepthWriteEnabled(true);
	SetDepthTest(EDepthTest::LESS);
	SetCullMode(ECullMode::CCW);
}

void Render::SetMaterial(Material *material)
{
	// Vertex shader
	if(material->vertexShader != nullptr)
	{
		if(material->vertexShader->renderData == nullptr)
		{
			Bind(material->vertexShader.get());
		}

		SetVertexShader(material->vertexShader.get());
	}

	// Fragment shader
	if(material->fragmentShader != nullptr)
	{
		if(material->fragmentShader->renderData == nullptr)
		{
			Bind(material->fragmentShader.get());
		}

		SetFragmentShader(material->fragmentShader.get());
	}

	// Textures
	uint8_t samplerIdx = 0;
	for(const Material::TextureInfo& textureInfo : material->GetTextures())
	{
		if(textureInfo.texture->renderData == nullptr)
		{
			Bind(textureInfo.texture.get());
		}

		SetTexture(samplerIdx, textureInfo.texture.get(), textureInfo.filterMode, textureInfo.addressMode);
		samplerIdx++;
	}
}
