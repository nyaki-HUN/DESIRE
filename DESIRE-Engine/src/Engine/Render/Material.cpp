#include "Engine/stdafx.h"
#include "Engine/Render/Material.h"

Material::ShaderParam::ShaderParam(HashedString name, const void* pParam)
	: name(name)
	, pParam(pParam)
{
	ASSERT(pParam != nullptr);
}

Material::ShaderParam::ShaderParam(HashedString name, std::function<void(float*)>&& func)
	: name(name)
	, paramFunc(std::move(func))
{
	ASSERT(paramFunc != nullptr);
}

const void* Material::ShaderParam::GetValue() const
{
	if(pParam != nullptr)
	{
		return pParam;
	}

	static float s_value[16] = {};
	paramFunc(s_value);

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

void Material::AddShaderParam(HashedString name, const void* pParam)
{
	shaderParams.EmplaceAdd(name, pParam);
}

void Material::AddShaderParam(HashedString name, std::function<void(float*)>&& func)
{
	shaderParams.EmplaceAdd(name, std::move(func));
}

void Material::RemoveAllShaderParams()
{
	shaderParams.Clear();
}

const Array<Material::ShaderParam>& Material::GetShaderParams() const
{
	return shaderParams;
}
