#pragma once

#include "Render/IRender.h"

#include <vector>

class Shader;
class Texture;

class Material
{
public:
	struct TextureInfo
	{
		std::shared_ptr<Texture> texture;
		IRender::EFilterMode filterMode;
		IRender::EAddressMode addressMode;
	};

	Material();
	~Material();

	const std::vector<TextureInfo>& GetTextures() const;
	void AddTexture(const std::shared_ptr<Texture>& texture, IRender::EFilterMode filterMode = IRender::EFilterMode::TRILINEAR, IRender::EAddressMode addressMode = IRender::EAddressMode::REPEAT);
	void ChangeTexture(uint8_t idx,  const std::shared_ptr<Texture>& texture);

	std::shared_ptr<Shader> vertexShader;
	std::shared_ptr<Shader> fragmentShader;

private:
	std::vector<TextureInfo> textures;
};
