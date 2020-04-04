#include "Engine/stdafx.h"
#include "Engine/Resource/Mesh.h"

#include "Engine/Render/Render.h"

Mesh::VertexDecl::VertexDecl(EAttrib attrib, int count, EAttribType type)
	: attrib(attrib)
	, type(type)
	, count(static_cast<uint8_t>(count))
{
	ASSERT(attrib != EAttrib::Num);
	ASSERT(type != EAttribType::Num);
	ASSERT(0 < count && count <= 4);
}

uint32_t Mesh::VertexDecl::GetSizeInBytes() const
{
	const uint32_t sizePerAttribType[] =
	{
		sizeof(float),
		sizeof(uint8_t),
	};
	DESIRE_CHECK_ARRAY_SIZE(sizePerAttribType, EAttribType::Num);

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
		Modules::Render->Unbind(this);
	}
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
