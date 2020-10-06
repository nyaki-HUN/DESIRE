#pragma once

#include "Engine/Core/Container/Array.h"
#include "Engine/Core/HashedString.h"

#include "Engine/Render/Render.h"

class Shader;
class Texture;

class Material
{
public:
	struct TextureInfo
	{
		std::shared_ptr<Texture> m_texture;
		Render::EFilterMode m_filterMode;
		Render::EAddressMode m_addressMode;
	};

	struct ShaderParam
	{
		const HashedString m_name;

		ShaderParam(HashedString name, const void* pParam);
		ShaderParam(HashedString name, std::function<void(float*)>&& func);
		const void* GetValue() const;

	private:
		const void* m_pParam = nullptr;
		std::function<void(float*)> m_paramFunc = nullptr;
	};

	Material();
	~Material();

	void AddTexture(const std::shared_ptr<Texture>& texture, Render::EFilterMode filterMode = Render::EFilterMode::Trilinear, Render::EAddressMode addressMode = Render::EAddressMode::Repeat);
	void ChangeTexture(uint8_t idx, const std::shared_ptr<Texture>& texture);
	const Array<TextureInfo>& GetTextures() const;

	void AddShaderParam(HashedString name, const void* pParam);
	void AddShaderParam(HashedString name, std::function<void(float*)>&& func);
	void RemoveAllShaderParams();
	const Array<ShaderParam>& Material::GetShaderParams() const;

	std::shared_ptr<Shader> m_vertexShader;
	std::shared_ptr<Shader> m_pixelShader;

private:
	Array<TextureInfo> m_textures;
	Array<ShaderParam> m_shaderParams;
};
