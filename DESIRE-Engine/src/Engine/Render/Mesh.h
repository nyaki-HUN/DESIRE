#pragma once

#include "Engine/Core/Container/Array.h"

class RenderData;

class Mesh
{
public:
	enum class EType : uint8_t
	{
		Static,			// Never updated
		Dynamic			// Sometimes updated (but can be used across multiple frames)
	};

	enum class EAttrib : uint8_t
	{
		Position,
		Normal,
		Tangent,
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

	enum class EAttribType : uint8_t
	{
		Float,
		Uint8,
	};

	struct VertexLayout
	{
		EAttrib attrib;
		uint8_t count;
		EAttribType type;

		uint32_t GetSizeInBytes() const
		{
			switch(type)
			{
				case EAttribType::Float:	return count * sizeof(float);
				case EAttribType::Uint8:	return count * sizeof(uint8_t);
			}
			return 0;
		}
	};

	Mesh(Array<VertexLayout>&& vertexLayout, uint32_t indexCount, uint32_t vertexCount);
	~Mesh();

	EType GetType() const;
	uint32_t GetNumIndices() const;
	uint32_t GetNumVertices() const;
	uint32_t GetIndexSize() const;
	uint32_t GetVertexSize() const;
	uint32_t GetSizeOfIndexData() const;
	uint32_t GetSizeOfVertexData() const;
	const Array<VertexLayout>& GetVertexLayout() const;

	// Render engine specific data set at bind
	RenderData* m_pRenderData = nullptr;

	std::unique_ptr<uint16_t[]> m_spIndices;
	std::unique_ptr<float[]> m_spVertices;

protected:
	EType m_type = EType::Static;

private:
	void SetNumIndices(uint32_t numIndices);
	void SetNumVertices(uint32_t numVertices);

	uint32_t m_numIndices = 0;
	uint32_t m_numVertices = 0;
	uint32_t m_vertexSize = 0;
	Array<VertexLayout> m_vertexLayout;
};

class DynamicMesh : public Mesh
{
public:
	DynamicMesh::DynamicMesh(Array<VertexLayout>&& vertexLayout, uint32_t indexCount, uint32_t vertexCount)
		: Mesh(std::move(vertexLayout), indexCount, vertexCount)
	{
		m_type = Mesh::EType::Dynamic;
	}

	bool m_isIndicesDirty = false;
	bool m_isVerticesDirty = false;
};
