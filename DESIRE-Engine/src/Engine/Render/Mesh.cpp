#include "Engine/stdafx.h"
#include "Engine/Render/Mesh.h"

#include "Engine/Render/Render.h"

Mesh::VertexLayout::VertexLayout(EAttrib attrib, int count, EAttribType type)
	: attrib(attrib)
	, type(type)
	, count(static_cast<uint8_t>(count))
{
	ASSERT(attrib != EAttrib::Num);
	ASSERT(type != EAttribType::Num);
	ASSERT(0 < count && count <= 4);
}

uint32_t Mesh::VertexLayout::GetSizeInBytes() const
{
	const uint32_t sizePerAttribType[] =
	{
		sizeof(float),
		sizeof(uint8_t),
	};
	DESIRE_CHECK_ARRAY_SIZE(sizePerAttribType, EAttribType::Num);

	return count * sizePerAttribType[(size_t)type];
}

Mesh::Mesh(std::initializer_list<Mesh::VertexLayout> vertexLayoutInitList, uint32_t indexCount, uint32_t vertexCount)
	: vertexLayout(vertexLayoutInitList)
	, numIndices(indexCount)
	, numVertices(vertexCount)
{
	vertexSize = 0;
	for(const VertexLayout& decl : vertexLayout)
	{
		vertexSize += decl.GetSizeInBytes();
	}

	if(numIndices != 0)
	{
		const uint32_t sizeInBytes = GetSizeOfIndexData();
		indices = std::make_unique<uint16_t[]>(sizeInBytes / sizeof(uint16_t));
		memset(indices.get(), 0, sizeInBytes);
	}

	if(numVertices != 0)
	{
		const uint32_t sizeInBytes = GetSizeOfVertexData();
		vertices = std::make_unique<float[]>(sizeInBytes / sizeof(float));
		memset(vertices.get(), 0, sizeInBytes);
	}
}

Mesh::~Mesh()
{
	Modules::Render->Unbind(*this);
}

Mesh& Mesh::operator =(Mesh&& otherMesh)
{
	Modules::Render->Unbind(*this);

	type = otherMesh.type;
	indices = std::move(otherMesh.indices);
	vertices = std::move(otherMesh.vertices);
	numIndices = otherMesh.numIndices;
	numVertices = otherMesh.numVertices;
	vertexSize = otherMesh.vertexSize;
	vertexLayout = std::move(otherMesh.vertexLayout);

	return *this;
}

Mesh::EType Mesh::GetType() const
{
	return type;
}

uint32_t Mesh::GetNumIndices() const
{
	return numIndices;
}

uint32_t Mesh::GetNumVertices() const
{
	return numVertices;
}

uint32_t Mesh::GetIndexSize() const
{
	return sizeof(uint16_t);
}

uint32_t Mesh::GetVertexSize() const
{
	return vertexSize;
}

uint32_t Mesh::GetSizeOfIndexData() const
{
	return numIndices * GetIndexSize();
}

uint32_t Mesh::GetSizeOfVertexData() const
{
	return numVertices * GetVertexSize();
}

const Array<Mesh::VertexLayout>& Mesh::GetVertexLayout() const
{
	return vertexLayout;
}
