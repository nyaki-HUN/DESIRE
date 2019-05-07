#include "Engine/stdafx.h"
#include "Engine/Render/Material.h"

Material::ShaderParam::ShaderParam(HashedString name, std::function<void(void*)>&& func)
	: name(name)
	, func(std::move(func))
{

}

const void* Material::ShaderParam::GetValue() const
{
	static float s_value[16] = {};

	if(func != nullptr)
	{
		func(s_value);
	}

	return s_value;
}

Material::Material()
{

}

Material::~Material()
{

}

void Material::AddTexture(const std::shared_ptr<Texture>& texture, Render::EFilterMode filterMode, Render::EAddressMode addressMode)
{
	textures.Add({ texture, filterMode, addressMode });
}

void Material::ChangeTexture(uint8_t idx, const std::shared_ptr<Texture>& texture)
{
	if(idx >= textures.Size())
	{
		return;
	}

	textures[idx].texture = texture;
}

const Array<Material::TextureInfo>& Material::GetTextures() const
{
	return textures;
}

void Material::AddShaderParam(HashedString name, std::function<void(void*)>&& func)
{
	shaderParams.EmplaceAdd(name, std::move(func));
}

const Array<Material::ShaderParam>& Material::GetShaderParams() const
{
	return shaderParams;
}
