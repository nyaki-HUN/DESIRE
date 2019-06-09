#include "Engine/stdafx.h"
#include "Engine/Resource/ShaderLoader/FileShaderLoader.h"
#include "Engine/Resource/Shader.h"
#include "Engine/Core/FS/IReadFile.h"
#include "Engine/Core/String/StackString.h"

Shader* FileShaderLoader::Load(const ReadFilePtr& file)
{
	StackString<DESIRE_MAX_PATH_LEN> name = file->GetFilename();

	// Remove extension
	size_t pos = name.FindLast('.');
	if(pos != String::kInvalidPos)
	{
		name.RemoveFrom(pos);
	}

	// Remove path
	pos = name.FindLast('/');
	if(pos != String::kInvalidPos)
	{
		name.RemoveFrom(0, pos + 1);
	}

	Shader* shader = new Shader(name);
	shader->data = file->ReadFileContent();

	return shader;
}
