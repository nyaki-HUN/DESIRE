#include "stdafx.h"
#include "Resource/Shader.h"
#include "Render/IRender.h"

Shader::Shader(const String& name)
	: renderData(nullptr)
	, name(name)
{

}

Shader::~Shader()
{
	IRender::Get()->Unbind(this);
}
