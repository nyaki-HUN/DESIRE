#include "stdafx.h"
#include "Resource/TextureLoader/StbImageLoader.h"
#include "Resource/Texture.h"
#include "Core/fs/IReadFile.h"

#define STBI_NO_TGA					// We have our own TGA loader
#define STBI_NO_FAILURE_STRINGS
#define STBI_NO_STDIO
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture* StbImageLoader::Load(const ReadFilePtr& file)
{
	stbi_io_callbacks callbacks;
	callbacks.read = [](void *file, char *data, int size)
	{
		return (int)static_cast<IReadFile*>(file)->ReadBuffer(data, (size_t)size);
	};

	callbacks.skip = [](void *file, int n)
	{
		static_cast<IReadFile*>(file)->Seek(n);
	};

	callbacks.eof = [](void *file)
	{
		return static_cast<IReadFile*>(file)->IsEof() ? 1 : 0;
	};

	int width = 0;
	int height = 0;
	int numComponents = 0;
	uint8_t *data = stbi_load_from_callbacks(&callbacks, file.get(), &width, &height, &numComponents, 0);

	Texture::EFormat format = Texture::EFormat::UNKNOWN;
	switch(numComponents)
	{
		case 1:	break;	// greyscale
		case 2: break;	// greyscale with alpha
		case 3: format = Texture::EFormat::RGB8; break;
		case 4: format = Texture::EFormat::RGBA8; break;
	}

	if(format == Texture::EFormat::UNKNOWN ||
		width <= 0 || width > UINT16_MAX ||
		height <= 0 || height > UINT16_MAX)
	{
		free(data);
		return nullptr;
	}

	Texture *texture = new Texture((uint16_t)width, (uint16_t)height, format);
	texture->data = SMemoryBuffer(data, (size_t)(width * height * numComponents));
	return texture;
}