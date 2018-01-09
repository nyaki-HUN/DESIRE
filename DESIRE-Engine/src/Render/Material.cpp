#include "stdafx.h"
#include "Render/Material.h"

Material::ShaderParam::ShaderParam(HashedString name, std::function<void(void *, size_t)>&& func)
	: name(name)
	, func(std::move(func))
{

}

void Material::ShaderParam::GetValue(void *buffer, size_t size) const
{
	if(func != nullptr)
	{
		func(buffer, size);
	}
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

void Material::AddShaderParam(const char *name, std::function<void(void *, size_t)>&& func)
{
	shaderParams.emplace_back(HashedString::CreateFromDynamicString(name), std::move(func));
}

const std::vector<Material::ShaderParam>& Material::GetShaderParams() const
{
	return shaderParams;
}
