#include "stdafx.h"
#include "Resource/Mesh.h"

Mesh::Mesh()
	: renderData(nullptr)
	, indices(nullptr)
	, vertices(nullptr)
	, sizeOfIndices(0)
	, sizeOfVertices(0)
{

}

Mesh::~Mesh()
{
	ASSERT(renderData == nullptr);

	free(indices);
	free(vertices);
}
