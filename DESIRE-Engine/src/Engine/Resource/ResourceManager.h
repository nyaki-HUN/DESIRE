#pragma once

#include "Engine/Core/FS/FilePtr_fwd.h"
#include "Engine/Core/Container/Array.h"
#include "Engine/Core/String/DynamicString.h"

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
	std::shared_ptr<Shader> GetShader(const String& filename, const String& defines = String::kEmptyString);
	std::shared_ptr<Texture> GetTexture(const String& filename);

	void ReloadMesh(const String& filename);
	void ReloadShader(const String& filename);
	void ReloadTexture(const String& filename);

	typedef std::unique_ptr<Mesh>(*MeshLoaderFunc_t)(const ReadFilePtr&);
	typedef std::unique_ptr<Shader>(*ShaderLoaderFunc_t)(const ReadFilePtr&);
	typedef std::unique_ptr<Texture>(*TextureLoaderFunc_t)(const ReadFilePtr&);

private:
	std::shared_ptr<Mesh> LoadMesh(const String& filename);
	std::shared_ptr<Shader> LoadShader(const String& filename);
	std::shared_ptr<Texture> LoadTexture(const String& filename);

	void CreateErrorTexture();

	std::map<DynamicString, std::weak_ptr<Mesh>> m_loadedMeshes;
	std::map<DynamicString, std::weak_ptr<Shader>> m_loadedShaders;
	std::map<DynamicString, std::weak_ptr<Texture>> m_loadedTextures;

	std::shared_ptr<Texture> m_errorTexture;

	// The loaders should be initialized in modules.cpp
	static const Array<MeshLoaderFunc_t> s_meshLoaders;
	static const Array<ShaderLoaderFunc_t> s_shaderLoaders;
	static const Array<TextureLoaderFunc_t> s_textureLoaders;
};
