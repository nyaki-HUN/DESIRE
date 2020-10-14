#include "Engine/stdafx.h"
#include "Engine/Render/Material.h"

Material::ShaderParam::ShaderParam(HashedString name, const void* pParam)
	: m_name(name)
	, m_pParam(pParam)
{
	ASSERT(pParam != nullptr);
}

Material::ShaderParam::ShaderParam(HashedString name, std::function<void(float*)>&& func)
	: m_name(name)
	, m_paramFunc(std::move(func))
{
	ASSERT(m_paramFunc != nullptr);
}

const void* Material::ShaderParam::GetValue() const
{
	if(m_pParam != nullptr)
	{
		return m_pParam;
	}

	static float s_value[16] = {};
	m_paramFunc(s_value);

	return s_value;
}

Material::Material()
{
}

Material::~Material()
{
}

void Material::AddTexture(const std::shared_ptr<Texture>& texture, EFilterMode filterMode, EAddressMode addressMode)
{
	m_textures.Add({ texture, filterMode, addressMode });
}

void Material::ChangeTexture(uint8_t idx, const std::shared_ptr<Texture>& texture)
{
	if(idx >= m_textures.Size())
	{
		return;
	}

	m_textures[idx].m_texture = texture;
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
