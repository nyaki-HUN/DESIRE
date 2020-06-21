#pragma once

#include "Engine/Core/Container/Array.h"

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

	struct VertexLayout
	{
		EAttrib attrib = EAttrib::Num;
		EAttribType type = EAttribType::Num;
		uint8_t count = 0;

		VertexLayout(EAttrib attrib, int count, EAttribType type);
		uint32_t GetSizeInBytes() const;
	};

	Mesh(EType meshType = EType::Static);
	~Mesh();

	uint32_t GetBytesOfIndexData() const	{ return numIndices * sizeof(uint16_t); }
	uint32_t GetBytesOfVertexData() const	{ return numVertices * stride; }

	void CalculateStrideFromVertexLayout();

	// Render engine specific data set at bind
	void* renderData = nullptr;

	// Index data
	std::unique_ptr<uint16_t[]> indices;
	uint32_t numIndices = 0;

	// Vertex data
	std::unique_ptr<float[]> vertices;
	uint32_t numVertices = 0;
	uint32_t stride = 0;
	Array<VertexLayout> vertexLayout;

	const EType type;
};

class DynamicMesh : public Mesh
{
public:
	DynamicMesh()
		: Mesh(Mesh::EType::Dynamic)
	{
	}

	uint32_t GetTotalBytesOfIndexData() const	{ return maxNumOfIndices * sizeof(uint16_t); }
	uint32_t GetTotalBytesOfVertexData() const	{ return maxNumOfVertices * stride; }

	uint32_t maxNumOfIndices = 0;
	uint32_t maxNumOfVertices = 0;

	uint32_t indexOffset = 0;
	uint32_t vertexOffset = 0;

	bool isIndexDataUpdateRequired = false;
	bool isVertexDataUpdateRequired = false;
};
