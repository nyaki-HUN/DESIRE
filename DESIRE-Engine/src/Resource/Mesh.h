#pragma once

class Mesh
{
public:
	enum class EType
	{
		STATIC,			// Never changed
		DYNAMIC,		// Sometimes changed
		TRANSIENT		// Changed every frame
	};

	Mesh(EType meshType = EType::STATIC);
	~Mesh();

	uint32_t GetSizeOfIndices() const;
	uint32_t GetSizeOfVertices() const;

	// Render engine specific data set at bind
	void *renderData;

	uint16_t *indices;
	float *vertices;
	uint32_t numIndices;
	uint32_t numVertices;
	uint32_t stride;

	EType type;
	bool isUpdateRequiredForDynamicMesh;
};
