#pragma once

#include "Engine/Core/MemoryBuffer.h"
#include "Engine/Core/String/DynamicString.h"

#include <set>

class Shader
{
public:
	Shader(const String& name);
	~Shader();

	// Render engine specific data set at bind
	void* pRenderData = nullptr;

	MemoryBuffer data;
	std::set<DynamicString> defines;
	const DynamicString name;
};
