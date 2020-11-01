#pragma once

#include "Engine/Core/FS/FilePtr_fwd.h"

class Mesh;

class AssimpLoader
{
public:
	static std::unique_ptr<Mesh> Load(const ReadFilePtr& spFile);
};
