#include "stdafx.h"
#include "Render/IRender.h"
#include "Render/Material.h"
#include "Core/math/vectormath.h"
#include "Resource/Mesh.h"
#include "Resource/Shader.h"
#include "Resource/Texture.h"

void IRender::RenderMesh(Mesh *mesh, Material *material)
{
	if(	mesh == nullptr ||
		material == nullptr ||
		material->vertexShader == nullptr ||
		material->pixelShader == nullptr)
	{
		return;
	}

	// Mesh
	if(mesh->renderData == nullptr)
	{
		Bind(mesh);
	}
	else if(mesh->type == Mesh::EType::DYNAMIC)
	{
		UpdateDynamicMesh(static_cast<DynamicMesh*>(mesh));
	}

	SetMesh(mesh);

	// Shaders
	if(material->vertexShader->renderData == nullptr)
	{
		Bind(material->vertexShader.get());
	}

	if(material->pixelShader->renderData == nullptr)
	{
		Bind(material->pixelShader.get());
	}

	SetShadersFromMaterial(material);

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

	// Render
	DoRender();
}

void IRender::SetDefaultRenderStates()
{
	SetColorWriteEnabled(true, true);
	SetDepthWriteEnabled(true);
	SetDepthTest(EDepthTest::LESS);
	SetCullMode(ECullMode::CCW);
}
