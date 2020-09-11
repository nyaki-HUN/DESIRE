#include "Engine/stdafx.h"
#include "Engine/Render/Shader.h"

#include "Engine/Render/Render.h"

Shader::Shader(const String& name)
	: name(name)
{
}

Shader::~Shader()
{
	Modules::Render->Unbind(*this);
}
