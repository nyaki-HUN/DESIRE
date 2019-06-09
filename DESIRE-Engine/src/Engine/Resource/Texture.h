#pragma once

#include "Engine/Core/MemoryBuffer.h"

class Texture
{
public:
	enum class EFormat
	{
		Unknown,

		R8,
		RG8,
		RGB8,
		RGBA8,

		RGBA32F,

		// Depth buffer formats
		D16,
		D24_S8,
		D32
	};

	Texture(uint16_t width, uint16_t height, EFormat format, uint8_t numMipMaps = 0);
	~Texture();

	bool IsDepthFormat() const;

	// Render engine specific data set at bind
	void* renderData = nullptr;

	const uint16_t width;
	const uint16_t height;
	const EFormat format;
	const uint8_t numMipMaps;

	MemoryBuffer data;
};
