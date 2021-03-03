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
	if(pParam)
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

void Material::AddTexture(const std::shared_ptr<Texture>& spTexture, EFilterMode filterMode, EAddressMode addressMode)
{
	m_textures.Add({ spTexture, filterMode, addressMode });
}

void Material::ChangeTexture(uint8_t idx, const std::shared_ptr<Texture>& spTexture)
{
	if(idx >= m_textures.Size())
	{
		return;
	}

	m_textures[idx].spTexture = spTexture;
}

const Array<Material::TextureInfo>& Material::GetTextures() const
{
	return m_textures;
}

void Material::AddShaderParam(HashedString name, const void* pParam)
{
	m_shaderParams.EmplaceAdd(name, pParam);
}

void Material::AddShaderParam(HashedString name, std::function<void(float*)>&& func)
{
	m_shaderParams.EmplaceAdd(name, std::move(func));
}

void Material::RemoveAllShaderParams()
{
	m_shaderParams.Clear();
}

const Array<Material::ShaderParam>& Material::GetShaderParams() const
{
	return m_shaderParams;
}
