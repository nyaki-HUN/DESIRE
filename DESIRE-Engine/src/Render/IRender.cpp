#include "stdafx.h"
#include "Render/IRender.h"
#include "Render/Material.h"
#include "Resource/Mesh.h"
#include "Resource/Shader.h"
#include "Resource/Texture.h"

void IRender::RenderMesh(Mesh *mesh, Material *material)
{
	if(mesh == nullptr || material == nullptr || material->shader == nullptr)
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

	// Shader
	if(material->shader->renderData == nullptr)
	{
		Bind(material->shader.get());
	}
	SetShader(material->shader.get());

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
