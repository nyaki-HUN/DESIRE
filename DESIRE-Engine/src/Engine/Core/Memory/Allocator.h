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

	static Allocator& GetDefaultAllocator();

private:
	// Prevent copy and move
	Allocator(const Allocator& other) = delete;
	Allocator(Allocator&& other) = delete;
	Allocator& operator=(const Allocator& other) = delete;
	Allocator& operator=(Allocator&& other) = delete;
};
