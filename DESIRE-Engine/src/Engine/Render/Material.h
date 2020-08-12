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
		std::shared_ptr<Texture> texture;
		Render::EFilterMode filterMode;
		Render::EAddressMode addressMode;
	};

	struct ShaderParam
	{
		const HashedString name;

		ShaderParam(HashedString name, const void* pParam);
		ShaderParam(HashedString name, std::function<void(float*)>&& func);
		const void* GetValue() const;

	private:
		const void* pParam = nullptr;
		std::function<void(float*)> paramFunc = nullptr;
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

	std::shared_ptr<Shader> vertexShader;
	std::shared_ptr<Shader> fragmentShader;

private:
	Array<TextureInfo> textures;
	Array<ShaderParam> shaderParams;
};
