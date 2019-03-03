#include "Engine/stdafx.h"
#include "Engine/Resource/ShaderLoader/FileShaderLoader.h"
#include "Engine/Resource/Shader.h"
#include "Engine/Core/fs/IReadFile.h"

Shader* FileShaderLoader::Load(const ReadFilePtr& file)
{
	// Remove path
	const String& filename = file->GetFilename();
	size_t pos = filename.FindLast('/');
	String name = (pos != String::kInvalidPos) ? filename.SubString(pos + 1) : filename;
	// Remove extension
	pos = name.FindLast('.');
	if(pos != String::kInvalidPos)
	{
		name.RemoveFrom(pos);
	}

	Shader *shader = new Shader(name);
	shader->data = file->ReadFileContent();

	return shader;
}
