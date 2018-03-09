#include "Engine/stdafx.h"
#include "Engine/Core/Modules.h"
#include "Engine/Resource/Shader.h"
#include "Engine/Render/Render.h"

Shader::Shader(const String& name)
	: name(name)
{

}

Shader::~Shader()
{
	if(renderData != nullptr)
	{
		Modules::Render->Unbind(this);
	}
}
