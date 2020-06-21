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

Mesh::Mesh(EType meshType)
	: type(meshType)
{
}

Mesh::~Mesh()
{
	if(renderData != nullptr)
	{
		Modules::Render->Unbind(this);
	}
}

void Mesh::CalculateStrideFromVertexLayout()
{
	stride = 0;
	for(const VertexLayout& decl : vertexLayout)
	{
		stride += decl.GetSizeInBytes();
	}
}
