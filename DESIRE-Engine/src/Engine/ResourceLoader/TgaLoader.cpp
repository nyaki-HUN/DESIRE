#include "Engine/stdafx.h"
#include "Engine/ResourceLoader/TgaLoader.h"

#include "Engine/Core/FS/IReadFile.h"

#include "Engine/Render/Texture.h"

#if DESIRE_PLATFORM_WINDOWS
	#include <PshPack1.h>
#endif	// #if DESIRE_PLATFORM_WINDOWS

struct TgaHeader
{
	enum class EImageType : uint8_t
	{
		ColorMapped		= 1,
		TrueColor		= 2,
		Grayscale		= 3,
		// RLE packed types
		RLE_ColorMapped	= 9,
		RLE_TrueColor	= 10,
		RLE_Grayscale	= 11,
	};

	enum EDescriptorFlag
	{
		DIRECTION_RIGHT_TO_LEFT = 0x10,
		DIRECTION_TOP_TO_BOTTOM = 0x20,
	};

	uint8_t sizeOfId;					// size of ID field that follows the header
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

#if DESIRE_PLATFORM_WINDOWS
	#include <PopPack.h>
#endif	// #if DESIRE_PLATFORM_WINDOWS

std::unique_ptr<Texture> TgaLoader::Load(const ReadFilePtr& file)
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

	const size_t numComponents = header.bitsPerPixel / 8;
	const size_t dataSize = numComponents * header.width * header.height;

	std::unique_ptr<uint8_t[]> data = std::make_unique<uint8_t[]>(dataSize);
	switch(header.imageType)
	{
		case TgaHeader::EImageType::TrueColor:
		{
			bytesRead = file->ReadBuffer(data.get(), dataSize);
			ASSERT(bytesRead == dataSize);
			break;
		}

		case TgaHeader::EImageType::RLE_TrueColor:
		{
			uint8_t rlePacket[5];
			uint8_t* pData = data.get();
			do
			{
				// Read the packed (header byte + color info)
				bytesRead = file->ReadBuffer(rlePacket, numComponents + 1);
				if(bytesRead != numComponents + 1)
				{
					return nullptr;
				}

				// Write color information
				memcpy(pData, &rlePacket[1], numComponents);
				pData += numComponents;

				const uint8_t packetHeader = rlePacket[0];
				if(packetHeader & 0x80)		// Check the type of the packet
				{
					// RLE packet
					const int numRepeat = (packetHeader & ~0x80);
					for(int i = 0; i < numRepeat; i++)
					{
						memcpy(pData, &rlePacket[1], numComponents);
						pData += numComponents;
					}
				}
				else
				{
					// RAW packet
					const size_t numRawColor = packetHeader;
					file->ReadBuffer(pData, numRawColor * numComponents);
					pData += numRawColor * numComponents;
				}
			} while(pData < data.get() + dataSize);
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
		const size_t rowSize = header.width * numComponents;
		DESIRE_STACKALLOCATE_ARRAY(uint8_t, pTmp, rowSize);
		for(uint16_t i = 0; i < header.height / 2; ++i)
		{
			uint8_t* pSrc = &data[i * rowSize];
			uint8_t* pDst = &data[(header.height - i - 1) * rowSize];
			memcpy(pTmp, pDst, rowSize);
			memcpy(pDst, pSrc, rowSize);
			memcpy(pSrc, pTmp, rowSize);
		}
	}

	const Texture::EFormat format = (numComponents == 3) ? Texture::EFormat::RGB8 : Texture::EFormat::RGBA8;
	return std::make_unique<Texture>(header.width, header.height, format, std::move(data));
}
