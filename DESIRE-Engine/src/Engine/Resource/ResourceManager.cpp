#include "Engine/stdafx.h"
#include "Engine/Resource/ResourceManager.h"

#include "Engine/Core/FS/FileSystem.h"
#include "Engine/Core/FS/IReadFile.h"
#include "Engine/Core/String/StackString.h"

#include "Engine/Render/Mesh.h"
#include "Engine/Render/Render.h"
#include "Engine/Render/Shader.h"
#include "Engine/Render/Texture.h"

ResourceManager::ResourceManager()
{
	CreateErrorTexture();
}

ResourceManager::~ResourceManager()
{
}

std::shared_ptr<Mesh> ResourceManager::GetMesh(const String& filename)
{
	auto it = m_loadedMeshes.find(filename);
	if(it != m_loadedMeshes.end() && !it->second.expired())
	{
		return it->second.lock();
	}

	std::shared_ptr<Mesh> mesh = LoadMesh(filename);

	if(it != m_loadedMeshes.end())
	{
		it->second = mesh;
	}
	else
	{
		m_loadedMeshes.emplace(filename, mesh);
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

	auto it = m_loadedShaders.find(shaderNameWithDefines);
	if(it != m_loadedShaders.end() && !it->second.expired())
	{
		return it->second.lock();
	}

	std::shared_ptr<Shader> shader = LoadShader(filename);

	if(it != m_loadedShaders.end())
	{
		it->second = shader;
	}
	else
	{
		m_loadedShaders.emplace(std::move(shaderNameWithDefines), shader);
	}

	return shader;
}

std::shared_ptr<Texture> ResourceManager::GetTexture(const String& filename)
{
	auto it = m_loadedTextures.find(filename);
	if(it != m_loadedTextures.end() && !it->second.expired())
	{
		return it->second.lock();
	}

	std::shared_ptr<Texture> texture = LoadTexture(filename);

	if(it != m_loadedTextures.end())
	{
		it->second = texture;
	}
	else
	{
		m_loadedTextures.emplace(filename, texture);
	}

	return texture;
}

void ResourceManager::ReloadMesh(const String& filename)
{
	auto it = m_loadedMeshes.find(filename);
	if(it == m_loadedMeshes.end())
	{
		return;
	}

	std::shared_ptr<Mesh> mesh = it->second.lock();
	if(mesh != nullptr)
	{
		std::shared_ptr<Mesh> newMesh = LoadMesh(filename);
		*mesh = std::move(*newMesh);
	}
}

void ResourceManager::ReloadTexture(const String& filename)
{
	auto it = m_loadedTextures.find(filename);
	if(it == m_loadedTextures.end())
	{
		return;
	}

	std::shared_ptr<Texture> texture = it->second.lock();
	if(texture != nullptr)
	{
		std::shared_ptr<Texture> newTexture = LoadTexture(filename);
		*texture = std::move(*newTexture);
	}
}

void ResourceManager::ReloadShader(const String& filename)
{
	for(auto& pair : m_loadedShaders)
	{
		std::shared_ptr<Shader> shader = pair.second.lock();
		if(shader != nullptr && shader->m_name == filename)
		{
			Modules::Render->Unbind(*shader);

			std::shared_ptr<Shader> newShader = LoadShader(filename);
			shader->m_data = std::move(newShader->m_data);
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
			std::unique_ptr<Mesh> mesh = loaderFunc(file);
			if(mesh != nullptr)
			{
				return std::move(mesh);
			}

			file->Seek(0, IReadFile::ESeekOrigin::Begin);
		}
	}

	LOG_ERROR("Failed to load mesh from: %s", filename.Str());
	return nullptr;
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
			std::unique_ptr<Shader> shader = loaderFunc(file);
			if(shader != nullptr)
			{
				return std::move(shader);
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
			std::unique_ptr<Texture> texture = loaderFunc(file);
			if(texture != nullptr)
			{
				return std::move(texture);
			}

			file->Seek(0, IReadFile::ESeekOrigin::Begin);
		}
	}

	LOG_ERROR("Failed to load texture from: %s", filename.Str());
	return m_errorTexture;
}

void ResourceManager::CreateErrorTexture()
{
	constexpr uint16_t kTextureSize = 128;
	constexpr uint32_t dataSize = kTextureSize * kTextureSize * Texture::GetBytesPerPixelForFormat(Texture::EFormat::RGBA8);
	std::unique_ptr<uint8_t[]> data = std::make_unique<uint8_t[]>(dataSize);

	uint32_t* pPixel = reinterpret_cast<uint32_t*>(data.get());
	for(uint16_t i = 0; i < kTextureSize * kTextureSize; ++i)
	{
		*pPixel = ((i % 26) < 14) ? 0xFFFF8000 : 0xFF000000;
		pPixel++;
	}

	m_errorTexture = std::make_shared<Texture>(kTextureSize, kTextureSize, Texture::EFormat::RGBA8, std::move(data));
}
