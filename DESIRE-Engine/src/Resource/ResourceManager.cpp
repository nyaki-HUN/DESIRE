#include "stdafx.h"
#include "Resource/ResourceManager.h"
#include "Core/fs/FileSystem.h"
#include "Core/fs/IReadFile.h"

ResourceManager::ResourceManager()
{

}

ResourceManager::~ResourceManager()
{

}

Mesh* ResourceManager::LoadMesh(const char *filename)
{
	HashedString filenameHash = HashedString::CreateFromDynamicString(filename);
	Mesh **meshPtr = loadedMeshes.Find(filenameHash);
	if(meshPtr != nullptr)
	{
		return *meshPtr;
	}

	ReadFilePtr file = FileSystem::Get()->Open(filename);

	for(MeshLoaderFunc_t loaderFunc : meshLoaders)
	{
		Mesh *mesh = loaderFunc(file);
		if(mesh != nullptr)
		{
			loadedMeshes.Insert(filenameHash, mesh);
			return mesh;
		}

		file->Seek(0, IReadFile::ESeekOrigin::BEGIN);
	}

	LOG_ERROR("Failed to load mesh from: %s", filename);
	return nullptr;
}

Texture* ResourceManager::LoadTexture(const char *filename)
{
	HashedString filenameHash = HashedString::CreateFromDynamicString(filename);
	Texture **texturePtr = loadedTextures.Find(filenameHash);
	if(texturePtr != nullptr)
	{
		return *texturePtr;
	}

	ReadFilePtr file = FileSystem::Get()->Open(filename);

	for(TextureLoaderFunc_t loaderFunc : textureLoaders)
	{
		Texture *texture = loaderFunc(file);
		if(texture != nullptr)
		{
			loadedTextures.Insert(filenameHash, texture);
			return texture;
		}

		file->Seek(0, IReadFile::ESeekOrigin::BEGIN);
	}

	LOG_ERROR("Failed to load texture from: %s", filename);
	return nullptr;
}
