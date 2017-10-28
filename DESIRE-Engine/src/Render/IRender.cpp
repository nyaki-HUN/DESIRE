#include "stdafx.h"
#include "Render/IRender.h"
#include "Render/Material.h"
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
	if(activeMesh != mesh)
	{
		if(mesh->renderData == nullptr)
		{
			Bind(mesh);
		}

		SetMesh(mesh);
		activeMesh = mesh;
	}

	// Material
	if(activeMaterial != material)
	{
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

			SetTexture(i, texture);
		}

		activeMaterial = material;
	}

	// Render
	DoRender();
}
