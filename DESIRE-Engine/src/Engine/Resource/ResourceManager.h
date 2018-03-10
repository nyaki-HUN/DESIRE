#pragma once

#include "Engine/Core/String.h"
#include "Engine/Core/fs/FilePtr_fwd.h"

#include <memory>
#include <vector>
#include <map>

class Mesh;
class Shader;
class Texture;

class ResourceManager
{
public:
	ResourceManager();
	~ResourceManager();

	std::shared_ptr<Mesh> GetMesh(const String& filename);
	std::shared_ptr<Shader> GetShader(const String& filename, const String& defines = String::emptyString);
	std::shared_ptr<Texture> GetTexture(const String& filename);

	void ReloadMesh(const String& filename);
	void ReloadShader(const String& filename);
	void ReloadTexture(const String& filename);

	typedef Mesh*(*MeshLoaderFunc_t)(const ReadFilePtr&);
	typedef Shader*(*ShaderLoaderFunc_t)(const ReadFilePtr&);
	typedef Texture*(*TextureLoaderFunc_t)(const ReadFilePtr&);

private:
	std::shared_ptr<Mesh> LoadMesh(const String& filename);
	std::shared_ptr<Shader> LoadShader(const String& filename);
	std::shared_ptr<Texture> LoadTexture(const String& filename);

	void CreateErrorTexture();

	std::map<String, std::weak_ptr<Mesh>> loadedMeshes;
	std::map<String, std::weak_ptr<Shader>> loadedShaders;
	std::map<String, std::weak_ptr<Texture>> loadedTextures;

	std::shared_ptr<Texture> errorTexture;

	// The loaders should be initialized in modules.cpp
	static const std::vector<MeshLoaderFunc_t> meshLoaders;
	static const std::vector<ShaderLoaderFunc_t> shaderLoaders;
	static const std::vector<TextureLoaderFunc_t> textureLoaders;
};
