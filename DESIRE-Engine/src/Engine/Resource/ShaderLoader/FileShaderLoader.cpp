#include "Engine/stdafx.h"
#include "Engine/Resource/ShaderLoader/FileShaderLoader.h"

#include "Engine/Core/FS/IReadFile.h"
#include "Engine/Core/String/StackString.h"

#include "Engine/Resource/Shader.h"

std::unique_ptr<Shader> FileShaderLoader::Load(const ReadFilePtr& file)
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

	std::unique_ptr<Shader> shader = std::make_unique<Shader>(name);
	shader->data = file->ReadAllAsBinary();

	return shader;
}
