#include "stdafx.h"
#include "Render/Material.h"
#include "Render/IRender.h"

Material::Material()
{

}

Material::~Material()
{
	IRender *render = IRender::Get();

	render->Unbind(vertexShader.get());
	render->Unbind(pixelShader.get());

	for(std::shared_ptr<Texture>& texture : textures)
	{
		render->Unbind(texture.get());
	}
}
