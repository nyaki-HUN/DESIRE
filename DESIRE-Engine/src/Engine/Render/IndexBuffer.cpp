#include "Engine/stdafx.h"
#include "Engine/Render/IndexBuffer.h"

#include "Engine/Render/Render.h"

IndexBuffer::IndexBuffer(uint32_t indexCount, uint32_t flags)
	: DeviceBuffer(flags)
	, m_numIndices(indexCount)
{
	m_spIndices = std::make_unique<uint16_t[]>(m_numIndices);
}

IndexBuffer::~IndexBuffer()
{
	Modules::Render->Unbind(*this);
}

const void* IndexBuffer::GetData() const
{
	return m_spIndices.get();
}

uint32_t IndexBuffer::GetDataSize() const
{
	return m_numIndices * GetIndexSize();
}

uint32_t IndexBuffer::GetNumIndices() const
{
	return m_numIndices;
}

uint32_t IndexBuffer::GetIndexSize() const
{
	return sizeof(uint16_t);
}
