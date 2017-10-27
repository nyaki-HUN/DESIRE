#pragma once

class Shader
{
public:
	Shader();

	// Render engine specific data set at bind
	void *renderData;
};
