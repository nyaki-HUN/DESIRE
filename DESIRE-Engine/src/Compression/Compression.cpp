#include "stdafx.h"
#include "Compression/Compression.h"

Compression::Compression()
{
	compressionLevel = GetMinCompressionLevel();
}

Compression::~Compression()
{

}

void Compression::InitStreamForCompression()
{
	ASSERT(false && "Not implemented");
}

void Compression::InitStreamForDecompression()
{
	ASSERT(false && "Not implemented");
}

void Compression::SetCompressionLevel(int level)
{
	const int minLevel = GetMinCompressionLevel();
	const int maxLevel = GetMaxCompressionLevel();

	compressionLevel = std::max(minLevel, std::min(level, maxLevel));
}

int Compression::GetMinCompressionLevel() const
{
	return 0;
}

int Compression::GetMaxCompressionLevel() const
{
	return 0;
}
