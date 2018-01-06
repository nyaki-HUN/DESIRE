#include "stdafx.h"
#include "Resource/Shader.h"
#include "Render/Render.h"

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
