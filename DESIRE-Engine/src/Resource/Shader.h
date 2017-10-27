#pragma once

#include "Core/SMemoryBuffer.h"

class Shader
{
public:
	Shader();

	// Render engine specific data set at bind
	void *renderData;

	SMemoryBuffer vertexShaderDataBuffer;
	SMemoryBuffer pixelShaderDataBuffer;
};
