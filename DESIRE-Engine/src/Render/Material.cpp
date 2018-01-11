#include "stdafx.h"
#include "Render/Material.h"

Material::ShaderParam::ShaderParam(String&& paramName, std::function<void(void*)>&& func)
	: name(std::move(paramName))
	, nameHash(HashedString::CreateFromDynamicString(name.c_str(), name.Length()))
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

void Material::AddShaderParam(String&& name, std::function<void(void*)>&& func)
{
	shaderParams.emplace_back(std::move(name), std::move(func));
}

const std::vector<Material::ShaderParam>& Material::GetShaderParams() const
{
	return shaderParams;
}
