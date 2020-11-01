#pragma once

#include "Engine/Core/FS/FilePtr_fwd.h"

class Texture;

class StbImageLoader
{
public:
	static std::unique_ptr<Texture> Load(const ReadFilePtr& spFile);
};
