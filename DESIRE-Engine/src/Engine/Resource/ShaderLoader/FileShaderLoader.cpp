#include "Engine/stdafx.h"
#include "Engine/Resource/ShaderLoader/FileShaderLoader.h"
#include "Engine/Resource/Shader.h"
#include "Engine/Core/fs/IReadFile.h"

Shader* FileShaderLoader::Load(const ReadFilePtr& file)
{
	const String& filename = file->GetFilename();
	const size_t pos = filename.FindLast('/');
	const String name = (pos != String::INVALID_POS) ? filename.SubString(pos + 1) : filename;

	Shader *shader = new Shader(name);
	shader->data = file->ReadFileContent();

	return shader;
}
