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

Mesh::Mesh(EType meshType, std::initializer_list<Mesh::VertexLayout> vertexLayoutInitList)
	: type(meshType)
	, vertexLayout(vertexLayoutInitList)
{
	stride = 0;
	for(const VertexLayout& decl : vertexLayout)
	{
		stride += decl.GetSizeInBytes();
	}
}

Mesh::~Mesh()
{
	if(pRenderData != nullptr)
	{
		Modules::Render->Unbind(this);
	}
}

const Array<Mesh::VertexLayout>& Mesh::GetVertexLayout() const
{
	return vertexLayout;
}

DynamicMesh::DynamicMesh(uint32_t indexCount, uint32_t vertexCount, std::initializer_list<Mesh::VertexLayout> vertexLayoutInitList)
	: Mesh(Mesh::EType::Dynamic, vertexLayoutInitList)
	, maxNumOfIndices(indexCount)
	, maxNumOfVertices(vertexCount)
{
	indices = std::make_unique<uint16_t[]>(maxNumOfIndices);
	memset(indices.get(), 0, GetTotalBytesOfIndexData());

	vertices = std::make_unique<float[]>(maxNumOfVertices * stride / sizeof(float));
	memset(vertices.get(), 0, GetTotalBytesOfVertexData());
}
