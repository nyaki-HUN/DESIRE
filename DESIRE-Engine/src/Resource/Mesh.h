#pragma once

#include <vector>

class Mesh
{
public:
	Mesh();
	~Mesh();

	// Render engine specific data set at bind
	void *renderData;

	std::vector<uint16_t> indices;
	std::vector<float> vertices;
};
