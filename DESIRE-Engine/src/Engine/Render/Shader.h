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
	void* m_pRenderData = nullptr;

	MemoryBuffer m_data;
	std::set<DynamicString> m_defines;
	const DynamicString m_name;
};
