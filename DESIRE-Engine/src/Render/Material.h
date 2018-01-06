#pragma once

#include "Render/Render.h"

#include <vector>

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

	Material();
	~Material();

	const std::vector<TextureInfo>& GetTextures() const;
	void AddTexture(const std::shared_ptr<Texture>& texture, Render::EFilterMode filterMode = Render::EFilterMode::TRILINEAR, Render::EAddressMode addressMode = Render::EAddressMode::REPEAT);
	void ChangeTexture(uint8_t idx,  const std::shared_ptr<Texture>& texture);

	std::shared_ptr<Shader> vertexShader;
	std::shared_ptr<Shader> fragmentShader;

private:
	std::vector<TextureInfo> textures;
};
