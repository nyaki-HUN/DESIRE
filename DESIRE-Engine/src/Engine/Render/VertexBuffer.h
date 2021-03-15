#pragma once

#include "Engine/Core/Container/Array.h"

#include "Engine/Render/DeviceBuffer.h"

class VertexBuffer : public DeviceBuffer
{
public:
	enum class EAttrib : uint8_t
	{
		Position,
		Normal,
		Tangent,
		Color,
		Texcoord0,
		Texcoord1,
		Texcoord2,
		Texcoord3,
		Texcoord4,
		Texcoord5,
		Texcoord6,
		Texcoord7,
		Num
	};

	enum class EAttribType : uint8_t
	{
		Float,
		Uint8,
	};

	struct Layout
	{
		EAttrib attrib;
		uint8_t count;
		EAttribType type;

		uint32_t GetSizeInBytes() const
		{
			switch(type)
			{
				case EAttribType::Float:	return count * sizeof(float);
				case EAttribType::Uint8:	return count * sizeof(uint8_t);
			}
			return 0;
		}
	};

	VertexBuffer(uint32_t vertexCount, Array<Layout>&& vertexLayout, uint32_t flags = 0);
	~VertexBuffer() override;

	const void* GetData() const override;
	uint32_t GetDataSize() const override;

	uint32_t GetNumVertices() const;
	uint32_t GetVertexSize() const;
	const Array<Layout>& GetVertexLayout() const;

	std::unique_ptr<float[]> m_spVertices;

private:
	uint32_t m_numVertices = 0;
	uint32_t m_vertexSize = 0;
	Array<Layout> m_vertexLayout;
};
