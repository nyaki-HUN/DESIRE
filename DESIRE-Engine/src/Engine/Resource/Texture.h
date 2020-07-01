#pragma once

#include "Engine/Core/MemoryBuffer.h"

class Texture
{
public:
	enum class EFormat
	{
		R8,
		RG8,
		RGB8,
		RGBA8,

		RGB32F,
		RGBA32F,

		// Depth buffer formats
		D16,
		D24_S8,
		D32
	};

	Texture(uint16_t width, uint16_t height, EFormat format, std::unique_ptr<uint8_t[]> data, uint8_t numMipLevels = 1);
	Texture(uint16_t width, uint16_t height, EFormat format, const void* pDataToCopy = nullptr, uint8_t numMipLevels = 1);
	~Texture();

	bool IsDepthFormat() const;
	uint32_t GetDataSize() const;
	uint8_t GetBytesPerPixel() const;

	// Render engine specific data set at bind
	void* renderData = nullptr;

	const uint16_t width;
	const uint16_t height;
	const EFormat format;
	const uint8_t numMipLevels;
	std::unique_ptr<uint8_t[]> data;
};
