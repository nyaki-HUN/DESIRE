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

	Mesh(std::initializer_list<Mesh::VertexLayout> vertexLayoutInitList, uint32_t indexCount, uint32_t vertexCount);
	~Mesh();

	Mesh& operator =(Mesh&& otherMesh);

	uint32_t GetNumIndices() const;
	uint32_t GetNumVertices() const;
	uint32_t GetSizeOfIndexData() const;
	uint32_t GetSizeOfVertexData() const;

	EType GetType() const;
	const Array<VertexLayout>& GetVertexLayout() const;

	// Render engine specific data set at bind
	void* pRenderData = nullptr;

	std::unique_ptr<uint16_t[]> indices;
	std::unique_ptr<float[]> vertices;
	uint32_t stride = 0;

protected:
	EType type = EType::Static;

private:
	uint32_t numIndices = 0;
	uint32_t numVertices = 0;
	Array<VertexLayout> vertexLayout;
};

class DynamicMesh : public Mesh
{
public:
	DynamicMesh::DynamicMesh(std::initializer_list<Mesh::VertexLayout> vertexLayoutInitList, uint32_t indexCount, uint32_t vertexCount)
		: Mesh(vertexLayoutInitList, indexCount, vertexCount)
	{
		type = Mesh::EType::Dynamic;
	}

	bool isIndicesDirty = false;
	bool isVerticesDirty = false;
};
