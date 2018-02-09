#pragma once

#include <memory>

class Mesh
{
public:
	enum class EType
	{
		STATIC,			// Never updated
		DYNAMIC			// Sometimes updated (but can be used across multiple frames)
	};

	enum class EAttrib
	{
		POSITION,
		NORMAL,
		COLOR,
		TEXCOORD0,
		TEXCOORD1,
		TEXCOORD2,
		TEXCOORD3,
		TEXCOORD4,
		TEXCOORD5,
		TEXCOORD6,
		TEXCOORD7,
		NUM
	};

	enum class EAttribType
	{
		FLOAT,
		UINT8,
		NUM
	};

	struct VertexDecl
	{
		EAttrib attrib = EAttrib::NUM;
		EAttribType type = EAttribType::NUM;
		uint8_t count = 0;

		VertexDecl(EAttrib attrib, int count, EAttribType type);
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
	std::unique_ptr<uint16_t[]> indices;
	uint32_t numIndices;

	// Vertex data
	std::unique_ptr<float[]> vertices;
	uint32_t numVertices;
	uint32_t stride;
	std::vector<VertexDecl> vertexDecl;

	const EType type;
};

class DynamicMesh : public Mesh
{
public:
	DynamicMesh()
		: Mesh(Mesh::EType::DYNAMIC)
		, maxNumOfIndices(0)
		, maxNumOfVertices(0)
		, indexOffset(0)
		, vertexOffset(0)
		, isIndexDataUpdateRequired(false)
		, isVertexDataUpdateRequired(false)
	{

	}

	uint32_t GetMaxSizeOfIndices() const
	{
		return maxNumOfIndices * sizeof(uint16_t);
	}

	uint32_t GetMaxSizeOfVertices() const
	{
		return maxNumOfVertices * stride;
	}

	uint32_t maxNumOfIndices;
	uint32_t maxNumOfVertices;

	uint32_t indexOffset;
	uint32_t vertexOffset;

	bool isIndexDataUpdateRequired;
	bool isVertexDataUpdateRequired;
};
