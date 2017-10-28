#include "stdafx.h"
#include "Render/IRender.h"
#include "Render/Material.h"
#include "Resource/Mesh.h"
#include "Resource/Shader.h"
#include "Resource/Texture.h"

void IRender::RenderMesh(Mesh *mesh, Material *material)
{
	if(mesh == nullptr || material == nullptr)
	{
		return;
	}

	// Mesh
	if(mesh->renderData == nullptr)
	{
		Bind(mesh);
	}
	SetMesh(mesh);
	activeMesh = mesh;

	// Shaders
	Shader *vertexShader = (material->vertexShader != nullptr) ? material->vertexShader.get() : errorVertexShader;
	if(vertexShader->renderData == nullptr)
	{
		Bind(vertexShader);
	}

	Shader *pixelShader = (material->pixelShader != nullptr) ? material->pixelShader.get() : errorPixelShader;
	if(pixelShader->renderData == nullptr)
	{
		Bind(pixelShader);
	}

	SetShader(vertexShader, pixelShader);

	// Textures
	for(uint8_t i = 0; i < material->textures.size(); i++)
	{
		Texture *texture = material->textures[i].get();
		if(texture->renderData == nullptr)
		{
			Bind(texture);
		}
		SetTexture(i, texture);
	}

	activeMaterial = material;

	// Render
	DoRender();
}
