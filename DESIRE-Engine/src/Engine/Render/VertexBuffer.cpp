#include "Engine/stdafx.h"
#include "Engine/Render/VertexBuffer.h"

#include "Engine/Render/Render.h"

VertexBuffer::VertexBuffer(uint32_t vertexCount, Array<Layout>&& vertexLayout, uint32_t flags)
	: DeviceBuffer(flags)
	, m_numVertices(vertexCount)
	, m_vertexLayout(std::move(vertexLayout))
{
	m_vertexSize = 0;
	for(const Layout& decl : m_vertexLayout)
	{
		m_vertexSize += decl.GetSizeInBytes();
	}

	m_spVertices = std::make_unique<float[]>(m_numVertices * m_vertexSize / sizeof(float));
}

VertexBuffer::~VertexBuffer()
{
	Modules::Render->Unbind(*this);
}

const void* VertexBuffer::GetData() const
{
	return m_spVertices.get();
}

uint32_t VertexBuffer::GetDataSize() const
{
	return m_numVertices * GetVertexSize();
}

uint32_t VertexBuffer::GetNumVertices() const
{
	return m_numVertices;
}

uint32_t VertexBuffer::GetVertexSize() const
{
	return m_vertexSize;
}

const Array<VertexBuffer::Layout>& VertexBuffer::GetVertexLayout() const
{
	return m_vertexLayout;
}
