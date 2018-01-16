#include "Engine/stdafx.h"
#include "Engine/Resource/ShaderLoader/FileShaderLoader.h"
#include "Engine/Resource/Shader.h"
#include "Engine/Core/fs/IReadFile.h"

Shader* FileShaderLoader::Load(const ReadFilePtr& file)
{
	String name = file->GetFilename();
	const size_t pos = name.FindLast('/');
	if(pos != String::INVALID_POS)
	{
		name.Remove(0, pos + 1);
	}

	Shader *shader = new Shader(name);
	shader->data = file->ReadFileContent();

	return shader;
}
