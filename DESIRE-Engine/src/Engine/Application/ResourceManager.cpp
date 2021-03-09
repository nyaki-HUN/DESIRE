#include "Engine/stdafx.h"
#include "Engine/Application/ResourceManager.h"

#include "Engine/Core/FS/FileSystem.h"
#include "Engine/Core/FS/IReadFile.h"
#include "Engine/Core/Object.h"
#include "Engine/Core/String/StackString.h"

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

std::unique_ptr<Object> ResourceManager::CreateObjectFromFile(const String& filename)
{
	std::unique_ptr<Object> spObejct = LoadObject(filename);
	return spObejct;
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

	std::shared_ptr<Shader> spShader = LoadShader(filename);
	if(!spShader)
	{
		spShader = std::make_unique<Shader>(filename);
	}

	if(it != m_loadedShaders.end())
	{
		it->second = spShader;
	}
	else
	{
		m_loadedShaders.emplace(std::move(shaderNameWithDefines), spShader);
	}

	return spShader;
}

std::shared_ptr<Texture> ResourceManager::GetTexture(const String& filename)
{
	auto it = m_loadedTextures.find(filename);
	if(it != m_loadedTextures.end() && !it->second.expired())
	{
		return it->second.lock();
	}

	std::shared_ptr<Texture> spTexture = LoadTexture(filename);
	if(!spTexture)
	{
		spTexture = m_spErrorTexture;
	}

	if(it != m_loadedTextures.end())
	{
		it->second = spTexture;
	}
	else
	{
		m_loadedTextures.emplace(filename, spTexture);
	}

	return spTexture;
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
		std::unique_ptr<Texture> spNewTexture = LoadTexture(filename);
		if(spNewTexture)
		{
			*spTexture = std::move(*spNewTexture);
		}
		else
		{
			*spTexture = *m_spErrorTexture;
		}
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

			std::unique_ptr<Shader> spNewShader = LoadShader(filename);
			if(spNewShader)
			{
				spShader->m_data = std::move(spNewShader->m_data);
			}
			else
			{
				spShader->m_data = MemoryBuffer();
			}
		}
	}
}

std::unique_ptr<Object> ResourceManager::LoadObject(const String& filename)
{
	for(ObjectLoaderFunc_t loaderFunc : s_objectLoaders)
	{
		std::unique_ptr<Object> spObject = loaderFunc(filename);
		if(spObject)
		{
			return spObject;
		}
	}

	LOG_ERROR("Failed to load object from: %s", filename.Str());
	return nullptr;
}

std::unique_ptr<Shader> ResourceManager::LoadShader(const String& filename)
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
				return spShader;
			}

			spFile->Seek(0, IReadFile::ESeekOrigin::Begin);
		}
	}

	LOG_ERROR("Failed to load shader from: %s", filenameWithPath.Str());
	return nullptr;
}

std::unique_ptr<Texture> ResourceManager::LoadTexture(const String& filename)
{
	ReadFilePtr spFile = FileSystem::Get().Open(filename);
	if(spFile)
	{
		for(TextureLoaderFunc_t loaderFunc : s_textureLoaders)
		{
			std::unique_ptr<Texture> spTexture = loaderFunc(spFile);
			if(spTexture)
			{
				return spTexture;
			}

			spFile->Seek(0, IReadFile::ESeekOrigin::Begin);
		}
	}

	LOG_ERROR("Failed to load texture from: %s", filename.Str());
	return nullptr;
}

void ResourceManager::CreateErrorTexture()
{
	constexpr uint16_t kTextureSize = 128;
	constexpr uint32_t kDataSize = kTextureSize * kTextureSize * Texture::GetBytesPerPixelForFormat(Texture::EFormat::RGBA8);
	std::unique_ptr<uint8_t[]> spData = std::make_unique<uint8_t[]>(kDataSize);

	uint32_t* pPixel = reinterpret_cast<uint32_t*>(spData.get());
	for(uint16_t i = 0; i < kTextureSize * kTextureSize; ++i)
	{
		*pPixel = ((i % 26) < 14) ? 0xFFFF8000 : 0xFF000000;
		pPixel++;
	}

	m_spErrorTexture = std::make_shared<Texture>(kTextureSize, kTextureSize, Texture::EFormat::RGBA8, std::move(spData));
}
