#pragma once

#include "Engine/Core/FS/FilePtr_fwd.h"

class Texture;

class TgaLoader
{
public:
	static std::unique_ptr<Texture> Load(const ReadFilePtr& file);
};
