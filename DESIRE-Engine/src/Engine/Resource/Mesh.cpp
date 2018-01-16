#include "Engine/stdafx.h"
#include "Engine/Resource/Mesh.h"
#include "Engine/Render/Render.h"

Mesh::VertexDecl::VertexDecl(EAttrib attrib, int count, EAttribType type)
	: attrib(attrib)
	, type(type)
	, count((uint8_t)count)
{
	ASSERT(attrib != EAttrib::NUM);
	ASSERT(type != EAttribType::NUM);
	ASSERT(0 < count && count <= 4);
}

uint32_t Mesh::VertexDecl::GetSizeInBytes() const
{
	static const uint32_t sizePerAttribType[] =
	{
		sizeof(float),
		sizeof(uint8_t),
	};
	DESIRE_CHECK_ARRAY_SIZE(sizePerAttribType, EAttribType::NUM);

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
	if(renderData != nullptr)
	{
		Render::Get()->Unbind(this);
	}

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
