#pragma once

#include "Engine/Core/fs/FilePtr_fwd.h"
#include "Engine/Core/Container/Array.h"
#include "Engine/Core/String/DynamicString.h"

#include <memory>
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

	std::map<DynamicString, std::weak_ptr<Mesh>> loadedMeshes;
	std::map<DynamicString, std::weak_ptr<Shader>> loadedShaders;
	std::map<DynamicString, std::weak_ptr<Texture>> loadedTextures;

	std::shared_ptr<Texture> errorTexture;

	// The loaders should be initialized in modules.cpp
	static const Array<MeshLoaderFunc_t> meshLoaders;
	static const Array<ShaderLoaderFunc_t> shaderLoaders;
	static const Array<TextureLoaderFunc_t> textureLoaders;
};
