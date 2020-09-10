#pragma once

// --------------------------------------------------------------------------------------------------------------------
//	Base class for memory allocators
// --------------------------------------------------------------------------------------------------------------------

class Allocator
{
public:
	Allocator() {}
	virtual ~Allocator() {}

	virtual void* Alloc(size_t size) = 0;
	virtual void* Realloc(void* ptr, size_t newSize, size_t oldSize);
	virtual void Free(void* ptr, size_t size) = 0;

	uint64_t GetAllocatedBytes() const;

	static Allocator& GetDefaultAllocator();

protected:
	std::atomic<uint64_t> allocatedBytes = 0;

private:
	DESIRE_NO_COPY_AND_MOVE(Allocator)
};
