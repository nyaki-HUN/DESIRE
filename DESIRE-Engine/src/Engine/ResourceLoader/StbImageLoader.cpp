#include "Engine/stdafx.h"
#include "Engine/ResourceLoader/StbImageLoader.h"

#include "Engine/Core/FS/IReadFile.h"
#include "Engine/Core/Memory/MemorySystem.h"

#include "Engine/Render/Texture.h"

DESIRE_DISABLE_WARNINGS
#define STBI_NO_TGA					// We have our own TGA loader
#define STBI_NO_FAILURE_STRINGS
#define STBI_NO_STDIO
#define STB_IMAGE_IMPLEMENTATION
#define STBI_MALLOC(sz)				MemorySystem::Alloc(sz)
#define STBI_REALLOC(p,newsz)		MemorySystem::Realloc(p, newsz)
#define STBI_FREE(p)				MemorySystem::Free(p)
#include "stb/stb_image.h"
DESIRE_ENABLE_WARNINGS

std::unique_ptr<Texture> StbImageLoader::Load(const ReadFilePtr& spFile)
{
	stbi_io_callbacks callbacks;
	callbacks.read = [](void* pFile, char* pData, int size)
	{
		const size_t numBytesRead = static_cast<IReadFile*>(pFile)->ReadBuffer(pData, static_cast<size_t>(size));
		return static_cast<int>(numBytesRead);
	};

	callbacks.skip = [](void* pFile, int n)
	{
		static_cast<IReadFile*>(pFile)->Seek(n);
	};

	callbacks.eof = [](void* pFile)
	{
		return static_cast<IReadFile*>(pFile)->IsEof() ? 1 : 0;
	};

	std::unique_ptr<uint8_t[]> spData = nullptr;
	Texture::EFormat format = Texture::EFormat::RGBA8;

	int width = 0;
	int height = 0;
	int numComponents = 0;
	const bool isHDR = stbi_is_hdr_from_callbacks(&callbacks, spFile.get());
	spFile->Seek(0, IReadFile::ESeekOrigin::Begin);
	if(isHDR)
	{
		spData = std::unique_ptr<uint8_t[]>(reinterpret_cast<uint8_t*>(stbi_loadf_from_callbacks(&callbacks, spFile.get(), &width, &height, &numComponents, 0)));
		switch(numComponents)
		{
			case 3:		format = Texture::EFormat::RGB32F; break;
			case 4:		format = Texture::EFormat::RGBA32F; break;
			default:	return nullptr;
		}
	}
	else
	{
		spData = std::unique_ptr<uint8_t[]>(stbi_load_from_callbacks(&callbacks, spFile.get(), &width, &height, &numComponents, 0));
		switch(numComponents)
		{
			case 1:		format = Texture::EFormat::R8; break;		// greyscale
			case 2:		format = Texture::EFormat::RG8; break;		// greyscale with alpha
			case 3:		format = Texture::EFormat::RGB8; break;
			case 4:		format = Texture::EFormat::RGBA8; break;
			default:	return nullptr;
		}
	}

	if(width <= 0 || height <= 0 || width > UINT16_MAX || height > UINT16_MAX)
	{
		return nullptr;
	}

	return std::make_unique<Texture>(static_cast<uint16_t>(width), static_cast<uint16_t>(height), format, std::move(spData));
}
