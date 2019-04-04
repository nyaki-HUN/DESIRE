#pragma once

#include "Engine/Core/Container/Array.h"

#include <memory>

class Mesh
{
public:
	enum class EType
	{
		Static,			// Never updated
		Dynamic			// Sometimes updated (but can be used across multiple frames)
	};

	enum class EAttrib
	{
		Position,
		Normal,
		Color,
		Texcoord0,
		Texcoord1,
		Texcoord2,
		Texcoord3,
		Texcoord4,
		Texcoord5,
		Texcoord6,
		Texcoord7,
		Num
	};

	enum class EAttribType
	{
		Float,
		Uint8,
		Num
	};

	struct VertexDecl
	{
		EAttrib attrib = EAttrib::Num;
		EAttribType type = EAttribType::Num;
		uint8_t count = 0;

		VertexDecl(EAttrib attrib, int count, EAttribType type);
		uint32_t GetSizeInBytes() const;
	};

	Mesh(EType meshType = EType::Static);
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
	Array<VertexDecl> vertexDecl;

	const EType type;
};

class DynamicMesh : public Mesh
{
public:
	DynamicMesh()
		: Mesh(Mesh::EType::Dynamic)
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
