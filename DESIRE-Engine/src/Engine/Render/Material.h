#pragma once

#include "Engine/Core/HashedString.h"
#include "Engine/Render/Render.h"

#include <memory>
#include <vector>
#include <functional>

class Shader;
class Texture;

class Material
{
public:
	struct TextureInfo
	{
		std::shared_ptr<Texture> texture;
		Render::EFilterMode filterMode;
		Render::EAddressMode addressMode;
	};

	struct ShaderParam
	{
		const HashedString name;

		ShaderParam(HashedString name, std::function<void(void*)>&& func);
		const void* GetValue() const;

	private:
		std::function<void(void *)> func;
	};

	Material();
	~Material();

	void AddTexture(const std::shared_ptr<Texture>& texture, Render::EFilterMode filterMode = Render::EFilterMode::TRILINEAR, Render::EAddressMode addressMode = Render::EAddressMode::REPEAT);
	void ChangeTexture(uint8_t idx,  const std::shared_ptr<Texture>& texture);
	const std::vector<TextureInfo>& GetTextures() const;

	void AddShaderParam(HashedString name, std::function<void(void*)>&& func);
	const std::vector<ShaderParam>& Material::GetShaderParams() const;

	std::shared_ptr<Shader> vertexShader;
	std::shared_ptr<Shader> fragmentShader;

private:
	std::vector<TextureInfo> textures;
	std::vector<ShaderParam> shaderParams;
};
