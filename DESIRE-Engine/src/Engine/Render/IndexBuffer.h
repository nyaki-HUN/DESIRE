#pragma once

#include "Engine/Render/DeviceBuffer.h"

class IndexBuffer : public DeviceBuffer
{
public:
	IndexBuffer(uint32_t indexCount, uint32_t flags = 0);
	~IndexBuffer() override;

	const void* GetData() const override;
	uint32_t GetDataSize() const override;

	uint32_t GetNumIndices() const;
	uint32_t GetIndexSize() const;

	std::unique_ptr<uint16_t[]> m_spIndices;

private:
	uint32_t m_numIndices = 0;
};
