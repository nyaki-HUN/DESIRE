#pragma once

#include "Core/fs/FilePtr_fwd.h"

class Texture;

class StbImageLoader
{
public:
	static Texture* Load(const ReadFilePtr& file);
};
