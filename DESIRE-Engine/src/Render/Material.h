#pragma once

#include <memory>

class Shader;
class Texture;

class Material
{
public:
	Material();
	~Material();

	std::shared_ptr<Shader> vertexShader;
	std::shared_ptr<Shader> pixelShader;
	std::vector<std::shared_ptr<Texture>> textures;
};
