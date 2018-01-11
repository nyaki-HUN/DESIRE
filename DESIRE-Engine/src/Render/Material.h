#pragma once

#include "Core/HashedString.h"
#include "Render/Render.h"

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
		const String name;
		const HashedString nameHash;

		ShaderParam(String&& paramName, std::function<void(void*)>&& func);
		const void* GetValue() const;

	private:
		std::function<void(void *)> func;
	};

	Material();
	~Material();

	void AddTexture(const std::shared_ptr<Texture>& texture, Render::EFilterMode filterMode = Render::EFilterMode::TRILINEAR, Render::EAddressMode addressMode = Render::EAddressMode::REPEAT);
	void ChangeTexture(uint8_t idx,  const std::shared_ptr<Texture>& texture);
	const std::vector<TextureInfo>& GetTextures() const;

	void AddShaderParam(String&& name, std::function<void(void*)>&& func);
	const std::vector<ShaderParam>& Material::GetShaderParams() const;

	std::shared_ptr<Shader> vertexShader;
	std::shared_ptr<Shader> fragmentShader;

private:
	std::vector<TextureInfo> textures;
	std::vector<ShaderParam> shaderParams;
};
