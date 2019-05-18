#pragma once

#include "Engine/Core/FS/FilePtr_fwd.h"

class Texture;

class StbImageLoader
{
public:
	static Texture* Load(const ReadFilePtr& file);
};
