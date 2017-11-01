#pragma once

#include "Core/SMemoryBuffer.h"
#include "Core/String.h"

class Shader
{
public:
	Shader(const String& name);
	~Shader();

	// Render engine specific data set at bind
	void *renderData;

	SMemoryBuffer data;
	const String name;
};
