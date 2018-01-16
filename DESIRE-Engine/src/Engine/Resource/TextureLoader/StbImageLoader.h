#pragma once

#include "Engine/Core/fs/FilePtr_fwd.h"

class Texture;

class StbImageLoader
{
public:
	static Texture* Load(const ReadFilePtr& file);
};
