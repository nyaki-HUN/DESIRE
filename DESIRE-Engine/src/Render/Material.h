#pragma once

#include <memory>
#include <vector>

class Shader;
class Texture;

class Material
{
public:
	Material();
	~Material();

	std::shared_ptr<Shader> vertexShader;
	std::shared_ptr<Shader> fragmentShader;
	std::vector<std::shared_ptr<Texture>> textures;
};
