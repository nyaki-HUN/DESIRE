#include "Engine/stdafx.h"
#include "Engine/Resource/ResourceManager.h"

#include "Engine/Core/FS/FileSystem.h"
#include "Engine/Core/FS/IReadFile.h"
#include "Engine/Core/String/StackString.h"

#include "Engine/Render/Render.h"

#include "Engine/Resource/Mesh.h"
#include "Engine/Resource/Shader.h"
#include "Engine/Resource/Texture.h"

ResourceManager::ResourceManager()
{
	CreateErrorTexture();
}

ResourceManager::~ResourceManager()
{
}

std::shared_ptr<Mesh> ResourceManager::GetMesh(const String& filename)
{
	auto it = loadedMeshes.find(filename);
	if(it != loadedMeshes.end() && !it->second.expired())
	{
		return it->second.lock();
	}

	std::shared_ptr<Mesh> mesh = LoadMesh(filename);

	if(it != loadedMeshes.end())
	{
		it->second = mesh;
	}
	else
	{
		loadedMeshes.emplace(filename, mesh);
	}

	return mesh;
}

std::shared_ptr<Shader> ResourceManager::GetShader(const String& filename, const String& defines)
{
	DynamicString shaderNameWithDefines;
	shaderNameWithDefines.Reserve(filename.Length() + 1 + defines.Length());
	shaderNameWithDefines += filename;
	shaderNameWithDefines += "|";
	shaderNameWithDefines += defines;

	auto it = loadedShaders.find(shaderNameWithDefines);
	if(it != loadedShaders.end() && !it->second.expired())
	{
		return it->second.lock();
	}

	std::shared_ptr<Shader> shader = LoadShader(filename);

	if(it != loadedShaders.end())
	{
		it->second = shader;
	}
	else
	{
		loadedShaders.emplace(std::move(shaderNameWithDefines), shader);
	}

	return shader;
}

std::shared_ptr<Texture> ResourceManager::GetTexture(const String& filename)
{
	auto it = loadedTextures.find(filename);
	if(it != loadedTextures.end() && !it->second.expired())
	{
		return it->second.lock();
	}

	std::shared_ptr<Texture> texture = LoadTexture(filename);

	if(it != loadedTextures.end())
	{
		it->second = texture;
	}
	else
	{
		loadedTextures.emplace(filename, texture);
	}

	return texture;
}

void ResourceManager::ReloadMesh(const String& filename)
{
	auto it = loadedMeshes.find(filename);
	if(it == loadedMeshes.end() || !it->second.expired())
	{
		return;
	}

	std::shared_ptr<Mesh> newMesh = LoadMesh(filename);
	if(newMesh != nullptr)
	{
		std::shared_ptr<Mesh> mesh = it->second.lock();
		Modules::Render->Unbind(mesh.get());

		mesh->indices = std::move(newMesh->indices);
		mesh->numIndices = newMesh->numIndices;
		mesh->vertices = std::move(newMesh->vertices);
		mesh->numVertices = newMesh->numVertices;
		mesh->stride = newMesh->stride;
		mesh->vertexDecl = std::move(newMesh->vertexDecl);
	}
}

void ResourceManager::ReloadTexture(const String& filename)
{
	auto it = loadedTextures.find(filename);
	if(it == loadedTextures.end() || !it->second.expired())
	{
		return;
	}

	std::shared_ptr<Texture> newTexture = LoadTexture(filename);
	if(newTexture != nullptr)
	{
		std::shared_ptr<Texture> texture = it->second.lock();
		Modules::Render->Unbind(texture.get());

		ASSERT(texture->width == newTexture->width);
		ASSERT(texture->height == newTexture->height);
		ASSERT(texture->format == newTexture->format);
		ASSERT(texture->numMipMaps == newTexture->numMipMaps);
		texture->data = std::move(newTexture->data);
	}
}

void ResourceManager::ReloadShader(const String& filename)
{
	for(auto& pair : loadedShaders)
	{
		std::shared_ptr<Shader> shader = pair.second.lock();
		if(shader != nullptr && shader->name == filename)
		{
			std::shared_ptr<Shader> newShader = LoadShader(filename);
			if(newShader != nullptr)
			{
				Modules::Render->Unbind(shader.get());

				shader->data = std::move(newShader->data);
			}
		}
	}
}

std::shared_ptr<Mesh> ResourceManager::LoadMesh(const String& filename)
{
	ReadFilePtr file = FileSystem::Get()->Open(filename);
	if(file)
	{
		for(MeshLoaderFunc_t loaderFunc : s_meshLoaders)
		{
			Mesh* mesh = loaderFunc(file);
			if(mesh != nullptr)
			{
				return std::shared_ptr<Mesh>(mesh);
			}

			file->Seek(0, IReadFile::ESeekOrigin::Begin);
		}
	}

	LOG_ERROR("Failed to load mesh from: %s", filename.Str());
	return std::make_shared<Mesh>();
}

std::shared_ptr<Shader> ResourceManager::LoadShader(const String& filename)
{
	StackString<DESIRE_MAX_PATH_LEN> filenameWithPath;
	Modules::Render->AppendShaderFilenameWithPath(filenameWithPath, filename);

	ReadFilePtr file = FileSystem::Get()->Open(filenameWithPath);
	if(file)
	{
		for(ShaderLoaderFunc_t loaderFunc : s_shaderLoaders)
		{
			Shader* shader = loaderFunc(file);
			if(shader != nullptr)
			{
				return std::shared_ptr<Shader>(shader);
			}

			file->Seek(0, IReadFile::ESeekOrigin::Begin);
		}
	}

	LOG_ERROR("Failed to load shader from: %s", filenameWithPath.Str());
	return std::make_shared<Shader>(filename);
}

std::shared_ptr<Texture> ResourceManager::LoadTexture(const String& filename)
{
	ReadFilePtr file = FileSystem::Get()->Open(filename);
	if(file)
	{
		for(TextureLoaderFunc_t loaderFunc : s_textureLoaders)
		{
			Texture* texture = loaderFunc(file);
			if(texture != nullptr)
			{
				return std::shared_ptr<Texture>(texture);
			}

			file->Seek(0, IReadFile::ESeekOrigin::Begin);
		}
	}

	LOG_ERROR("Failed to load texture from: %s", filename.Str());
	return errorTexture;
}

void ResourceManager::CreateErrorTexture()
{
	const uint16_t textureSize = 128;
	errorTexture = std::make_shared<Texture>(textureSize, textureSize, Texture::EFormat::RGBA8);
	errorTexture->data = MemoryBuffer(textureSize * textureSize * 4);
	uint32_t* pixel = reinterpret_cast<uint32_t*>(errorTexture->data.ptr.get());
	for(int i = 0; i < textureSize * textureSize; ++i)
	{
		*pixel = ((i % 26) < 14) ? 0xFFFF8000 : 0xFF000000;
		pixel++;
	}
}
