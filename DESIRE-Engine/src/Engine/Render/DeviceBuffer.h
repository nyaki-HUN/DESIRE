#pragma once

class RenderData;

class DeviceBuffer
{
public:
	enum EFlags
	{
		STATIC		= 1 << 0,
		DYNAMIC		= 1 << 1,
		DIRTY		= 1 << 2
	};

	DeviceBuffer(uint32_t flags);
	virtual ~DeviceBuffer();

	uint32_t GetFlags() const;
	void SetDirty(bool isDirty = true);

	virtual const void* GetData() const = 0;
	virtual uint32_t GetDataSize() const = 0;

	// Render engine specific data set at bind
	RenderData* m_pRenderData = nullptr;

private:
	uint32_t m_flags = 0;
};
