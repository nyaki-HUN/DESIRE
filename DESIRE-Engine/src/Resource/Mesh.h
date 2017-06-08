#pragma once

class Mesh
{
public:
	Mesh();
	~Mesh();

	// Render engine specific data set at bind
	void *renderData;

	uint16_t *indices;
	float *vertices;
	uint32_t sizeOfIndices;
	uint32_t sizeOfVertices;
};
