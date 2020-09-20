#include "Engine/stdafx.h"
#include "Engine/Render/Renderable.h"

#include "Engine/Render/Render.h"

Renderable::~Renderable()
{
	Modules::Render->Unbind(*this);
}
