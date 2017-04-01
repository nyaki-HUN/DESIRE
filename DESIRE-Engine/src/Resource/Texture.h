#pragma once

#include "Core/SMemoryBuffer.h"

class Texture
{
public:
	enum EFormat
	{
		UNKNOWN,
		RGB8,
		RGBA8
	};

	Texture(uint16_t width, uint16_t height, EFormat format, uint8_t numMipMaps = 0);
	~Texture();

	// Render engine specific data set at bind
	void *renderData;

	const uint16_t width;
	const uint16_t height;
	const EFormat format;
	const uint8_t numMipMaps;

	SMemoryBuffer data;
};
