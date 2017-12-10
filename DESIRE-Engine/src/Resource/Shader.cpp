#include "stdafx.h"
#include "Resource/Shader.h"
#include "Render/IRender.h"

Shader::Shader(const String& name)
	: name(name)
{

}

Shader::~Shader()
{
	IRender::Get()->Unbind(this);
}
