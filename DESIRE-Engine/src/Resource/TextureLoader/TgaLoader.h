#pragma once

#include "Core/fs/FilePtr_fwd.h"

class Texture;

class TgaLoader
{
public:
	static Texture* Load(const ReadFilePtr& file);
};
