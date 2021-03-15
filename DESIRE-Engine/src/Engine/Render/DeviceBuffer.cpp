#include "Engine/stdafx.h"
#include "Engine/Render/DeviceBuffer.h"

DeviceBuffer::DeviceBuffer(uint32_t flags)
	: m_flags(flags)
{
}

DeviceBuffer::~DeviceBuffer()
{
	ASSERT(m_pRenderData == nullptr);
}

uint32_t DeviceBuffer::GetFlags() const
{
	return m_flags;
}

void DeviceBuffer::SetDirty(bool isDirty)
{
	if(isDirty)
	{
		m_flags |= DIRTY;
	}
	else
	{
		m_flags &= ~DIRTY;
	}
}
