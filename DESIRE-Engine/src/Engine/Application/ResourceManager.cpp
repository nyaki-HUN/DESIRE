#include "Engine/stdafx.h"
#include "Engine/Application/ResourceManager.h"

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

	std::shared_ptr<Mesh> spMesh = it->second.lock();
	if(spMesh)
	{
		std::shared_ptr<Mesh> newMesh = LoadMesh(filename);
		*spMesh = std::move(*newMesh);
	}
}

void ResourceManager::ReloadTexture(const String& filename)
{
	auto it = m_loadedTextures.find(filename);
	if(it == m_loadedTextures.end())
	{
		return;
	}

	std::shared_ptr<Texture> spTexture = it->second.lock();
	if(spTexture)
	{
		std::shared_ptr<Texture> newTexture = LoadTexture(filename);
		*spTexture = std::move(*newTexture);
	}
}

void ResourceManager::ReloadShader(const String& filename)
{
	for(auto& pair : m_loadedShaders)
	{
		std::shared_ptr<Shader> spShader = pair.second.lock();
		if(spShader && spShader->m_name == filename)
		{
			Modules::Render->Unbind(*spShader);

			std::shared_ptr<Shader> spNewShader = LoadShader(filename);
			spShader->m_data = std::move(spNewShader->m_data);
		}
	}
}

std::shared_ptr<Mesh> ResourceManager::LoadMesh(const String& filename)
{
	ReadFilePtr spFile = FileSystem::Get().Open(filename);
	if(spFile)
	{
		for(MeshLoaderFunc_t loaderFunc : s_meshLoaders)
		{
			std::unique_ptr<Mesh> spMesh = loaderFunc(spFile);
			if(spMesh)
			{
				return std::move(spMesh);
			}

			spFile->Seek(0, IReadFile::ESeekOrigin::Begin);
		}
	}

	LOG_ERROR("Failed to load mesh from: %s", filename.Str());
	return nullptr;
}

std::shared_ptr<Shader> ResourceManager::LoadShader(const String& filename)
{
	StackString<DESIRE_MAX_PATH_LEN> filenameWithPath;
	Modules::Render->AppendShaderFilenameWithPath(filenameWithPath, filename);

	ReadFilePtr spFile = FileSystem::Get().Open(filenameWithPath);
	if(spFile)
	{
		for(ShaderLoaderFunc_t loaderFunc : s_shaderLoaders)
		{
			std::unique_ptr<Shader> spShader = loaderFunc(spFile);
			if(spShader)
			{
				return std::move(spShader);
			}

			spFile->Seek(0, IReadFile::ESeekOrigin::Begin);
		}
	}

	LOG_ERROR("Failed to load shader from: %s", filenameWithPath.Str());
	return std::make_shared<Shader>(filename);
}

std::shared_ptr<Texture> ResourceManager::LoadTexture(const String& filename)
{
	ReadFilePtr spFile = FileSystem::Get().Open(filename);
	if(spFile)
	{
		for(TextureLoaderFunc_t loaderFunc : s_textureLoaders)
		{
			std::unique_ptr<Texture> spTexture = loaderFunc(spFile);
			if(spTexture)
			{
				return std::move(spTexture);
			}

			spFile->Seek(0, IReadFile::ESeekOrigin::Begin);
		}
	}

	LOG_ERROR("Failed to load texture from: %s", filename.Str());
	return m_spErrorTexture;
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

	m_spErrorTexture = std::make_shared<Texture>(kTextureSize, kTextureSize, Texture::EFormat::RGBA8, std::move(data));
}
