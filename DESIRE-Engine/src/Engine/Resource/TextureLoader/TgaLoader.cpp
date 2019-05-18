#include "Engine/stdafx.h"
#include "Engine/Resource/TextureLoader/TgaLoader.h"
#include "Engine/Resource/Texture.h"
#include "Engine/Core/FS/IReadFile.h"

#if defined(DESIRE_PLATFORM_WINDOWS)
	#include <PshPack1.h>
#endif

struct TgaHeader
{
	enum class EImageType : uint8_t
	{
		ColorMapped = 1,
		TrueColor = 2,
		Grayscale = 3,
		// RLE packed types
		RLE_ColorMapped = 9,
		RLE_TrueColor = 10,
		RLE_Grayscale = 11,
	};

	enum EDescriptorFlag
	{
		DIRECTION_RIGHT_TO_LEFT = 0x10,
		DIRECTION_TOP_TO_BOTTOM = 0x20,
	};

	uint8_t sizeOfID;					// size of ID field that follows the header
	uint8_t colorMapType;				// 0=none, 1=has color map
	EImageType imageType;

	// Color map specification
	uint16_t colorMapFirstEntryIndex;	// index of first color map entry that is included in the file
	uint16_t colorMapLength;			// number of colors in palette
	uint8_t colorMapEntrySize;			// number of bits per pixel (15, 16, 24, 32)

	// Image specification
	uint16_t xOrigin;
	uint16_t yOigin;
	uint16_t width;
	uint16_t height;
	uint8_t bitsPerPixel;
	uint8_t descriptor;					// bits 0-3 give the alpha channel depth, bits 4-5 give direction
} DESIRE_ATTRIBUTE_PACKED;

#if defined(DESIRE_PLATFORM_WINDOWS)
	#include <PopPack.h>
#endif

Texture* TgaLoader::Load(const ReadFilePtr& file)
{
	TgaHeader header;
	size_t bytesRead = file->ReadBuffer(&header, sizeof(header));

	if(bytesRead != sizeof(header) ||
		header.colorMapType != 0 ||
		(header.imageType != TgaHeader::EImageType::TrueColor && header.imageType != TgaHeader::EImageType::RLE_TrueColor) ||
		(header.bitsPerPixel != 24 && header.bitsPerPixel != 32))
	{
		// Not supported
		return nullptr;
	}

	const uint8_t numComponents = header.bitsPerPixel / 8u;
	const size_t dataSize = (size_t)(header.width * header.height * numComponents);

	uint8_t *data = (uint8_t*)malloc(dataSize);
	switch(header.imageType)
	{
		case TgaHeader::EImageType::TrueColor:
		{
			bytesRead = file->ReadBuffer(data, dataSize);
			ASSERT(bytesRead == dataSize);
			break;
		}

		case TgaHeader::EImageType::RLE_TrueColor:
		{
			uint8_t rlePacket[5];
			uint8_t *dataPtr = data;
			do
			{
				// Read the packed (header byte + color info)
				bytesRead = file->ReadBuffer(rlePacket, 1u + numComponents);
				if(bytesRead != 1u + numComponents)
				{
					free(data);
					return nullptr;
				}

				// Write color information
				memcpy(dataPtr, &rlePacket[1], numComponents);
				dataPtr += numComponents;

				const uint8_t packetHeader = rlePacket[0];
				if(packetHeader & 0x80)		// Check the type of the packet
				{
					// RLE packet
					const int numRepeat = (packetHeader & ~0x80);
					for(int i = 0; i < numRepeat; i++)
					{
						memcpy(dataPtr, &rlePacket[1], numComponents);
						dataPtr += numComponents;
					}
				}
				else
				{
					// RAW packet
					const size_t numRawColor = packetHeader;
					file->ReadBuffer(dataPtr, numRawColor * numComponents);
					dataPtr += numRawColor * numComponents;
				}
			} while(dataPtr < data + dataSize);
			break;
		}

		case TgaHeader::EImageType::ColorMapped:
		case TgaHeader::EImageType::Grayscale:
		case TgaHeader::EImageType::RLE_ColorMapped:
		case TgaHeader::EImageType::RLE_Grayscale:
			ASSERT(false && "Not supported");
			break;
	}

	// Check vertical direction
	if(header.descriptor & TgaHeader::DIRECTION_RIGHT_TO_LEFT)
	{
		ASSERT(false && "Not supported");
	}

	// Check horizontal direction
	if(!(header.descriptor & TgaHeader::DIRECTION_TOP_TO_BOTTOM))
	{
		// Flip the image horizontally
		const size_t rowSize = (size_t)(header.width * numComponents);
		void *tmp = malloc(rowSize);
		for(uint16_t i = 0; i < header.height / 2u; ++i)
		{
			uint8_t *src = data + i * rowSize;
			uint8_t *dst = data + (header.height - i - 1) * rowSize;
			memcpy(tmp, dst, rowSize);
			memcpy(dst, src, rowSize);
			memcpy(src, tmp, rowSize);
		}
		free(tmp);
	}

	Texture::EFormat format = (numComponents == 3) ? Texture::EFormat::RGB8 : Texture::EFormat::RGBA8;

	Texture *texture = new Texture(header.width, header.height, format);
	texture->data = MemoryBuffer(data, dataSize);
	return texture;
}
