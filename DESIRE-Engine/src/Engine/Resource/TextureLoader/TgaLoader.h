#pragma once

#include "Engine/Core/FS/FilePtr_fwd.h"

class Texture;

class TgaLoader
{
public:
	static Texture* Load(const ReadFilePtr& file);
};
