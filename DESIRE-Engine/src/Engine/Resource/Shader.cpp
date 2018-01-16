#include "Engine/stdafx.h"
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
		Render::Get()->Unbind(this);
	}
}
