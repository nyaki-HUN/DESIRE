#pragma once

class Mesh
{
public:
	enum class EType
	{
		STATIC,			// Never updated
		DYNAMIC,		// Sometimes updated (but can be used across multiple frames)
		TRANSIENT		// Updated in each frame
	};

	Mesh(EType meshType = EType::STATIC);
	~Mesh();

	uint32_t GetSizeOfIndices() const;
	uint32_t GetSizeOfVertices() const;

	// Render engine specific data set at bind
	void *renderData;

	// Index data
	uint16_t *indices;
	uint32_t numIndices;

	// Vertex data
	float *vertices;
	uint32_t numVertices;
	uint32_t stride;

	const EType type;
	bool isUpdateRequiredForDynamicMesh;
};
