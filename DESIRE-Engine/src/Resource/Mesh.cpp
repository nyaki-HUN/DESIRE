#include "stdafx.h"
#include "Resource/Mesh.h"

Mesh::Mesh(EType meshType)
	: renderData(nullptr)
	, indices(nullptr)
	, numIndices(0)
	, vertices(nullptr)
	, numVertices(0)
	, stride(0)
	, type(meshType)
	, isUpdateRequiredForDynamicMesh(false)
{

}

Mesh::~Mesh()
{
	ASSERT(renderData == nullptr && "Call IRender::Unbind() before destroying the Mesh");

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
