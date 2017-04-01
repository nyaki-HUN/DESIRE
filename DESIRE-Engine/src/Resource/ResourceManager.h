#pragma once

#include "Core/Singleton.h"
#include "Core/HashedStringMap.h"
#include "Core/fs/FilePtr_fwd.h"

#include <vector>

class Mesh;
class Texture;

class ResourceManager
{
	DESIRE_DECLARE_SINGLETON(ResourceManager)

public:
	Mesh* LoadMesh(const char *filename);
	Texture* LoadTexture(const char *filename);

	typedef Mesh*(*MeshLoaderFunc_t)(const ReadFilePtr&);
	typedef Texture*(*TextureLoaderFunc_t)(const ReadFilePtr&);

private:
	HashedStringMap<Mesh*> loadedMeshes;
	HashedStringMap<Texture*> loadedTextures;

	// The loaders should be initialized in modules.cpp
	static const std::vector<MeshLoaderFunc_t> meshLoaders;
	static const std::vector<TextureLoaderFunc_t> textureLoaders;
};
