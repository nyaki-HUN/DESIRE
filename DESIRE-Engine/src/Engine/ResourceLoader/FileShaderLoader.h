#pragma once

#include "Engine/Core/FS/FilePtr_fwd.h"

class Shader;

class FileShaderLoader
{
public:
	static std::unique_ptr<Shader> Load(const ReadFilePtr& spFile);
};
