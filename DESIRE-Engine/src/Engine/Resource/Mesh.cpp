#include "Engine/stdafx.h"
#include "Engine/Resource/Mesh.h"

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
	stride = 0;
	for(const VertexLayout& decl : vertexLayout)
	{
		stride += decl.GetSizeInBytes();
	}

	if(numIndices != 0)
	{
		indices = std::make_unique<uint16_t[]>(numIndices);
		memset(indices.get(), 0, GetSizeOfIndexData());
	}

	if(numVertices != 0)
	{
		vertices = std::make_unique<float[]>(numVertices * stride / sizeof(float));
		memset(vertices.get(), 0, GetSizeOfVertexData());
	}
}

Mesh::~Mesh()
{
	Modules::Render->Unbind(*this);
}

Mesh& Mesh::operator =(Mesh&& otherMesh)
{
	Modules::Render->Unbind(*this);

	numIndices = otherMesh.numIndices;
	numVertices = otherMesh.numVertices;
	indices = std::move(otherMesh.indices);
	vertices = std::move(otherMesh.vertices);
	type = otherMesh.type;
	vertexLayout = std::move(otherMesh.vertexLayout);

	return *this;
}

uint32_t Mesh::GetNumIndices() const
{
	return numIndices;
}

uint32_t Mesh::GetNumVertices() const
{
	return numVertices;
}

uint32_t Mesh::GetSizeOfIndexData() const
{
	return numIndices * sizeof(uint16_t); 
}

uint32_t Mesh::GetSizeOfVertexData() const
{
	return numVertices * stride; 
}

Mesh::EType Mesh::GetType() const
{
	return type;
}

const Array<Mesh::VertexLayout>& Mesh::GetVertexLayout() const
{
	return vertexLayout;
}
