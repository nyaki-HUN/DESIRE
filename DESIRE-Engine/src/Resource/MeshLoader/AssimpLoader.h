#pragma once

#include "Core/fs/FilePtr_fwd.h"

class Mesh;

class AssimpLoader
{
public:
	static Mesh* Load(const ReadFilePtr& file);
};
