#include "Engine/stdafx.h"
#include "Engine/Render/Mesh.h"

#include "Engine/Render/Render.h"

Mesh::Mesh(Array<VertexLayout>&& vertexLayout, uint32_t indexCount, uint32_t vertexCount)
	: m_vertexLayout(std::move(vertexLayout))
{
	m_vertexSize = 0;
	for(const VertexLayout& decl : m_vertexLayout)
	{
		m_vertexSize += decl.GetSizeInBytes();
	}

	SetNumIndices(indexCount);
	SetNumVertices(vertexCount);
}

Mesh::~Mesh()
{
	Modules::Render->Unbind(*this);
}

Mesh::EType Mesh::GetType() const
{
	return m_type;
}

uint32_t Mesh::GetNumIndices() const
{
	return m_numIndices;
}

uint32_t Mesh::GetNumVertices() const
{
	return m_numVertices;
}

uint32_t Mesh::GetIndexSize() const
{
	return sizeof(uint16_t);
}

uint32_t Mesh::GetVertexSize() const
{
	return m_vertexSize;
}

uint32_t Mesh::GetSizeOfIndexData() const
{
	return m_numIndices * GetIndexSize();
}

uint32_t Mesh::GetSizeOfVertexData() const
{
	return m_numVertices * GetVertexSize();
}

const Array<Mesh::VertexLayout>& Mesh::GetVertexLayout() const
{
	return m_vertexLayout;
}

void Mesh::SetNumIndices(uint32_t numIndices)
{
	if(m_numIndices != numIndices)
	{
		m_numIndices = numIndices;
		const uint32_t sizeInBytes = GetSizeOfIndexData();
		m_spIndices = std::make_unique<uint16_t[]>(sizeInBytes / sizeof(uint16_t));
		memset(m_spIndices.get(), 0, sizeInBytes);
	}
}

void Mesh::SetNumVertices(uint32_t numVertices)
{
	if(m_numVertices != numVertices)
	{
		m_numVertices = numVertices;
		const uint32_t sizeInBytes = GetSizeOfVertexData();
		m_spVertices = std::make_unique<float[]>(sizeInBytes / sizeof(float));
		memset(m_spVertices.get(), 0, sizeInBytes);
	}
}
