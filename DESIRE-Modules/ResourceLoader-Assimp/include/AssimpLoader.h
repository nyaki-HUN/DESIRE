#pragma once

#include "Engine/Core/FS/FilePtr_fwd.h"

class Mesh;

class AssimpLoader
{
public:
	static Mesh* Load(const ReadFilePtr& file);
};
