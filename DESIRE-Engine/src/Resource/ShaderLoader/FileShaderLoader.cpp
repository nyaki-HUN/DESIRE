#include "stdafx.h"
#include "Resource/ShaderLoader/FileShaderLoader.h"
#include "Resource/Shader.h"
#include "Core/fs/IReadFile.h"

Shader* FileShaderLoader::Load(const ReadFilePtr& file)
{
	Shader *shader = new Shader(file->GetFilename());
	shader->data = file->ReadFileContent();

	return shader;
}
