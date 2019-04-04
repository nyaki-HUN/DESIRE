#pragma once

#include "Engine/Core/Container/Array.h"
#include "Engine/Core/HashedString.h"
#include "Engine/Render/Render.h"

#include <memory>
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

	void AddTexture(const std::shared_ptr<Texture>& texture, Render::EFilterMode filterMode = Render::EFilterMode::Trilinear, Render::EAddressMode addressMode = Render::EAddressMode::Repeat);
	void ChangeTexture(uint8_t idx,  const std::shared_ptr<Texture>& texture);
	const Array<TextureInfo>& GetTextures() const;

	void AddShaderParam(HashedString name, std::function<void(void*)>&& func);
	const Array<ShaderParam>& Material::GetShaderParams() const;

	std::shared_ptr<Shader> vertexShader;
	std::shared_ptr<Shader> fragmentShader;

private:
	Array<TextureInfo> textures;
	Array<ShaderParam> shaderParams;
};
