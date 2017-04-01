#include "stdafx.h"
#include "Resource/Mesh.h"

Mesh::Mesh()
	: renderData(nullptr)
{

}

Mesh::~Mesh()
{
	ASSERT(renderData == nullptr);
}
