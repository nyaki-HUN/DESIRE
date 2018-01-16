#include "Engine/stdafx.h"
#include "Engine/Render/Material.h"

Material::ShaderParam::ShaderParam(HashedString name, std::function<void(void*)>&& func)
	: name(name)
	, func(std::move(func))
{

}

const void* Material::ShaderParam::GetValue() const
{
	static float value[16] = {};

	if(func != nullptr)
	{
		func(value);
	}

	return value;
}

Material::Material()
{

}

Material::~Material()
{

}

void Material::AddTexture(const std::shared_ptr<Texture>& texture, Render::EFilterMode filterMode, Render::EAddressMode addressMode)
{
	textures.push_back({ texture, filterMode, addressMode });
}

void Material::ChangeTexture(uint8_t idx, const std::shared_ptr<Texture>& texture)
{
	if(idx >= textures.size())
	{
		return;
	}

	textures[idx].texture = texture;
}

const std::vector<Material::TextureInfo>& Material::GetTextures() const
{
	return textures;
}

void Material::AddShaderParam(HashedString name, std::function<void(void*)>&& func)
{
	shaderParams.emplace_back(name, std::move(func));
}

const std::vector<Material::ShaderParam>& Material::GetShaderParams() const
{
	return shaderParams;
}
