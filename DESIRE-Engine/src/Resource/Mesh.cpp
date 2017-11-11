#include "stdafx.h"
#include "Resource/Mesh.h"
#include "Render/IRender.h"

uint32_t Mesh::VertexDecl::GetSizeInBytes() const
{
	static const uint32_t sizePerAttribType[] =
	{
		sizeof(float),
		sizeof(uint8_t),
	};
	DESIRE_CHECK_ARRAY_SIZE(sizePerAttribType, EAttribType);

	return count * sizePerAttribType[(size_t)type];
}

Mesh::Mesh(EType meshType)
	: renderData(nullptr)
	, indices(nullptr)
	, numIndices(0)
	, vertices(nullptr)
	, numVertices(0)
	, type(meshType)
{

}

Mesh::~Mesh()
{
	IRender::Get()->Unbind(this);

	free(indices);
	free(vertices);
}

uint32_t Mesh::GetSizeOfIndices() const
{
	return numIndices * sizeof(uint16_t);
}

uint32_t Mesh::GetSizeOfVertices() const
{
	return numVertices * stride;
}

void Mesh::CalculateStrideFromVertexDecl()
{
	stride = 0;
	for(const VertexDecl& decl : vertexDecl)
	{
		stride += decl.GetSizeInBytes();
	}
}
