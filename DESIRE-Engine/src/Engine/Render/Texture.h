#pragma once

#include "Engine/Core/MemoryBuffer.h"

class RenderData;

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

	Texture(uint16_t width, uint16_t height, EFormat format, std::unique_ptr<uint8_t[]> dataToMove, uint8_t numMipLevels = 1);
	Texture(uint16_t width, uint16_t height, EFormat format, const void* pDataToCopy = nullptr, uint8_t numMipLevels = 1);
	~Texture();

	Texture& operator =(Texture&& otherTexture);

	uint16_t GetWidth() const;
	uint16_t GetHeight() const;
	EFormat GetFormat() const;
	uint8_t GetNumMipLevels() const;
	const uint8_t* GetData() const;
	uint32_t GetDataSize() const;

	bool IsDepthFormat() const;
	uint8_t GetBytesPerPixel() const;

	static constexpr uint8_t GetBytesPerPixelForFormat(EFormat format)
	{
		switch(format)
		{
			case EFormat::R8:			return 1;
			case EFormat::RG8:			return 2;
			case EFormat::RGB8:			return 3;
			case EFormat::RGBA8:		return 4;
			case EFormat::RGB32F:		return 3 * 4;
			case EFormat::RGBA32F:		return 4 * 4;
			case EFormat::D16:			return 2;
			case EFormat::D24_S8:		return 4;
			case EFormat::D32:			return 4;
		}
		return 0;
	}

	// Render engine specific data set at bind
	RenderData* m_pRenderData = nullptr;

private:
	uint16_t m_width;
	uint16_t m_height;
	EFormat m_format;
	uint8_t m_numMipLevels;
	std::unique_ptr<uint8_t[]> m_data;
};
