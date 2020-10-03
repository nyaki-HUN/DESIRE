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
		EAttrib m_attrib = EAttrib::Num;
		EAttribType m_type = EAttribType::Num;
		uint8_t m_count = 0;

		VertexLayout(EAttrib attrib, int count, EAttribType type);
		uint32_t GetSizeInBytes() const;
	};

	Mesh(std::initializer_list<Mesh::VertexLayout> vertexLayoutInitList, uint32_t indexCount, uint32_t vertexCount);
	~Mesh();

	Mesh& operator =(Mesh&& otherMesh);

	EType GetType() const;
	uint32_t GetNumIndices() const;
	uint32_t GetNumVertices() const;
	uint32_t GetIndexSize() const;
	uint32_t GetVertexSize() const;
	uint32_t GetSizeOfIndexData() const;
	uint32_t GetSizeOfVertexData() const;
	const Array<VertexLayout>& GetVertexLayout() const;

	// Render engine specific data set at bind
	void* m_pRenderData = nullptr;

	std::unique_ptr<uint16_t[]> m_indices;
	std::unique_ptr<float[]> m_vertices;

protected:
	EType m_type = EType::Static;

private:
	uint32_t m_numIndices = 0;
	uint32_t m_numVertices = 0;
	uint32_t m_vertexSize = 0;
	Array<VertexLayout> m_vertexLayout;
};

class DynamicMesh : public Mesh
{
public:
	DynamicMesh::DynamicMesh(std::initializer_list<Mesh::VertexLayout> vertexLayoutInitList, uint32_t indexCount, uint32_t vertexCount)
		: Mesh(vertexLayoutInitList, indexCount, vertexCount)
	{
		m_type = Mesh::EType::Dynamic;
	}

	bool m_isIndicesDirty = false;
	bool m_isVerticesDirty = false;
};
