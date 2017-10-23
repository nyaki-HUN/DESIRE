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

	enum class EAttrib
	{
		POSITION,
		NORMAL,
		TEXCOORD0,
		TEXCOORD1,
		TEXCOORD2,
		COLOR,
		NUM
	};

	enum class EAttribType
	{
		FLOAT,
		UINT8,
		NUM
	};

	struct SVertexDecl
	{
		EAttrib attrib;
		EAttribType type;
		uint32_t count;

		SVertexDecl(EAttrib attrib, uint32_t count, EAttribType type)
			: attrib(attrib)
			, type(type)
			, count(count)
		{

		}

		uint32_t GetSizeInBytes() const;
	};

	Mesh(EType meshType = EType::STATIC);
	~Mesh();

	uint32_t GetSizeOfIndices() const;
	uint32_t GetSizeOfVertices() const;

	void CalculateStrideFromVertexDecl();

	// Render engine specific data set at bind
	void *renderData;

	// Index data
	uint16_t *indices;
	uint32_t numIndices;

	// Vertex data
	float *vertices;
	uint32_t numVertices;
	uint32_t stride;
	std::vector<SVertexDecl> vertexDecl;

	const EType type;
	bool isUpdateRequiredForDynamicMesh;
};
