#include "stdafx.h"
#include "Render/IRender.h"
#include "Render/Material.h"
#include "Core/math/vectormath.h"
#include "Resource/Mesh.h"
#include "Resource/Shader.h"
#include "Resource/Texture.h"

IRender::IRender()
{
	screenSpaceQuadVertexShader = std::make_unique<Shader>("vs_screenSpaceQuad");
}

IRender::~IRender()
{

}

void IRender::RenderMesh(Mesh *mesh, Material *material)
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
	UpdateShaderParams();

	DoRender();
}

void IRender::RenderScreenSpaceQuad(Material *material)
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
	UpdateShaderParams();

	DoRender();
}

void IRender::SetDefaultRenderStates()
{
	SetColorWriteEnabled(true, true);
	SetDepthWriteEnabled(true);
	SetDepthTest(EDepthTest::LESS);
	SetCullMode(ECullMode::CCW);
}

void IRender::SetMaterial(Material *material)
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
	for(uint8_t i = 0; i < material->textures.size(); i++)
	{
		Texture *texture = material->textures[i].get();
		if(texture->renderData == nullptr)
		{
			Bind(texture);
		}

		SetTexture(i, texture, EFilterMode::TRILINEAR, EAddressMode::REPEAT);
	}
}
