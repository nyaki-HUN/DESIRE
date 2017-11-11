#include "stdafx.h"
#include "Resource/ErrorTexture.h"

ErrorTexture::ErrorTexture()
	: Texture(128, 128, EFormat::RGBA8)
{
	FillData();
}

void ErrorTexture::FillData()
{
	data = MemoryBuffer(128 * 128 * 4);
	uint32_t *pixel = reinterpret_cast<uint32_t*>(data.data);
	for(int i = 0; i < width * height; ++i)
	{
		*pixel = ((i % 26) < 14) ? 0xFFFF8000 : 0xFF000000;
		pixel++;
	}
}
