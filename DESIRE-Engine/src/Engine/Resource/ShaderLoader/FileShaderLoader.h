#pragma once

#include "Engine/Core/fs/FilePtr_fwd.h"

class Shader;

class FileShaderLoader
{
public:
	static Shader* Load(const ReadFilePtr& file);
};
