#pragma once

#include "Core/MemoryBuffer.h"
#include "Core/String.h"

class Shader
{
public:
	Shader(const String& name);
	~Shader();

	// Render engine specific data set at bind
	void *renderData = nullptr;

	MemoryBuffer data;
	const String name;
};
