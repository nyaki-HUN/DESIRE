#include "Engine/stdafx.h"
#include "Engine/Render/Mesh.h"

#include "Engine/Render/Render.h"

Mesh::Mesh(std::initializer_list<Mesh::VertexLayout> vertexLayoutInitList, uint32_t indexCount, uint32_t vertexCount)
	: m_vertexLayout(vertexLayoutInitList)
	, m_numIndices(indexCount)
	, m_numVertices(vertexCount)
{
	m_vertexSize = 0;
	for(const VertexLayout& decl : m_vertexLayout)
	{
		m_vertexSize += decl.GetSizeInBytes();
	}

	if(m_numIndices != 0)
	{
		const uint32_t sizeInBytes = GetSizeOfIndexData();
		m_indices = std::make_unique<uint16_t[]>(sizeInBytes / sizeof(uint16_t));
		memset(m_indices.get(), 0, sizeInBytes);
	}

	if(m_numVertices != 0)
	{
		const uint32_t sizeInBytes = GetSizeOfVertexData();
		m_vertices = std::make_unique<float[]>(sizeInBytes / sizeof(float));
		memset(m_vertices.get(), 0, sizeInBytes);
	}
}

Mesh::~Mesh()
{
	Modules::Render->Unbind(*this);
}

Mesh& Mesh::operator =(Mesh&& otherMesh)
{
	Modules::Render->Unbind(*this);

	m_type = otherMesh.m_type;
	m_indices = std::move(otherMesh.m_indices);
	m_vertices = std::move(otherMesh.m_vertices);
	m_numIndices = otherMesh.m_numIndices;
	m_numVertices = otherMesh.m_numVertices;
	m_vertexSize = otherMesh.m_vertexSize;
	m_vertexLayout = std::move(otherMesh.m_vertexLayout);

	return *this;
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
