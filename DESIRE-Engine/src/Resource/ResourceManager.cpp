#include "stdafx.h"
#include "Resource/ResourceManager.h"
#include "Resource/Mesh.h"
#include "Resource/Shader.h"
#include "Resource/Texture.h"
#include "Core/fs/FileSystem.h"
#include "Core/fs/IReadFile.h"

ResourceManager::ResourceManager()
{

}

ResourceManager::~ResourceManager()
{

}

std::shared_ptr<Mesh> ResourceManager::GetMesh(const char *filename)
{
	HashedString filenameHash = HashedString::CreateFromDynamicString(filename);
	std::weak_ptr<Mesh> *meshPtr = loadedMeshes.Find(filenameHash);
	if(meshPtr != nullptr && !meshPtr->expired())
	{
		return meshPtr->lock();
	}

	std::shared_ptr<Mesh> mesh = LoadMesh(filename);

	if(meshPtr != nullptr)
	{
		*meshPtr = mesh;
	}
	else
	{
		loadedMeshes.Insert(filenameHash, mesh);
	}

	return mesh;
}

std::shared_ptr<Shader> ResourceManager::GetShader(const char *filename)
{
	HashedString filenameHash = HashedString::CreateFromDynamicString(filename);
	std::weak_ptr<Shader> *shaderPtr = loadedShaders.Find(filenameHash);
	if(shaderPtr != nullptr && !shaderPtr->expired())
	{
		return shaderPtr->lock();
	}

	std::shared_ptr<Shader> shader = LoadShader(filename);

	if(shaderPtr != nullptr)
	{
		*shaderPtr = shader;
	}
	else
	{
		loadedShaders.Insert(filenameHash, shader);
	}

	return shader;
}

std::shared_ptr<Texture> ResourceManager::GetTexture(const char *filename)
{
	HashedString filenameHash = HashedString::CreateFromDynamicString(filename);
	std::weak_ptr<Texture> *texturePtr = loadedTextures.Find(filenameHash);
	if(texturePtr != nullptr && !texturePtr->expired())
	{
		return texturePtr->lock();
	}

	std::shared_ptr<Texture> texture = LoadTexture(filename);

	if(texturePtr != nullptr)
	{
		*texturePtr = texture;
	}
	else
	{
		loadedTextures.Insert(filenameHash, texture);
	}

	return texture;
}

std::shared_ptr<Mesh> ResourceManager::LoadMesh(const char *filename)
{
	ReadFilePtr file = FileSystem::Get()->Open(filename);
	if(file)
	{
		for(MeshLoaderFunc_t loaderFunc : meshLoaders)
		{
			Mesh *mesh = loaderFunc(file);
			if(mesh != nullptr)
			{
				return std::shared_ptr<Mesh>(mesh);
			}

			file->Seek(0, IReadFile::ESeekOrigin::BEGIN);
		}
	}

	LOG_ERROR("Failed to load mesh from: %s", filename);
	return nullptr;
}

std::shared_ptr<Shader> ResourceManager::LoadShader(const char *filename)
{
	ReadFilePtr file = FileSystem::Get()->Open(filename);
	if(file)
	{
		for(ShaderLoaderFunc_t loaderFunc : shaderLoaders)
		{
			Shader *shader = loaderFunc(file);
			if(shader != nullptr)
			{
				return std::shared_ptr<Shader>(shader);
			}

			file->Seek(0, IReadFile::ESeekOrigin::BEGIN);
		}
	}

	LOG_ERROR("Failed to load texture from: %s", filename);
	return nullptr;
}

std::shared_ptr<Texture> ResourceManager::LoadTexture(const char *filename)
{
	ReadFilePtr file = FileSystem::Get()->Open(filename);
	if(file)
	{
		for(TextureLoaderFunc_t loaderFunc : textureLoaders)
		{
			Texture *texture = loaderFunc(file);
			if(texture != nullptr)
			{
				return std::shared_ptr<Texture>(texture);
			}

			file->Seek(0, IReadFile::ESeekOrigin::BEGIN);
		}
	}

	LOG_ERROR("Failed to load texture from: %s", filename);
	return nullptr;
}
