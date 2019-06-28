#pragma once

// --------------------------------------------------------------------------------------------------------------------
//	Base class for memory allocators with a default implementation that is using the system memory
// --------------------------------------------------------------------------------------------------------------------

class Allocator
{
public:
	Allocator();
	virtual ~Allocator();

	virtual void* Alloc(size_t size);
	virtual void* Realloc(void* ptr, size_t size);
	virtual void Free(void* ptr);

	static Allocator& GetDefaultAllocator();

private:
	// Prevent copy and move
	Allocator(const Allocator& other) = delete;
	Allocator(Allocator&& other) = delete;
	Allocator& operator=(const Allocator& other) = delete;
	Allocator& operator=(Allocator&& other) = delete;
};
