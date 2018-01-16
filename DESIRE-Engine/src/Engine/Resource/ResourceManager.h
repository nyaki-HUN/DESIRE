#pragma once

#include "Engine/Core/Singleton.h"
#include "Engine/Core/HashedStringMap.h"
#include "Engine/Core/fs/FilePtr_fwd.h"

#include <memory>
#include <vector>

class Mesh;
class Shader;
class Texture;

class ResourceManager
{
	DESIRE_SINGLETON(ResourceManager)

public:
	std::shared_ptr<Mesh> GetMesh(const char *filename);
	std::shared_ptr<Shader> GetShader(const char *filename, const char *defines = "");
	std::shared_ptr<Texture> GetTexture(const char *filename);

	typedef Mesh*(*MeshLoaderFunc_t)(const ReadFilePtr&);
	typedef Shader*(*ShaderLoaderFunc_t)(const ReadFilePtr&);
	typedef Texture*(*TextureLoaderFunc_t)(const ReadFilePtr&);

private:
	std::shared_ptr<Mesh> LoadMesh(const char *filename);
	std::shared_ptr<Shader> LoadShader(const char *filename);
	std::shared_ptr<Texture> LoadTexture(const char *filename);

	HashedStringMap<std::weak_ptr<Mesh>> loadedMeshes;
	HashedStringMap<std::weak_ptr<Shader>> loadedShaders;
	HashedStringMap<std::weak_ptr<Texture>> loadedTextures;

	// The loaders should be initialized in modules.cpp
	static const std::vector<MeshLoaderFunc_t> meshLoaders;
	static const std::vector<ShaderLoaderFunc_t> shaderLoaders;
	static const std::vector<TextureLoaderFunc_t> textureLoaders;
};
