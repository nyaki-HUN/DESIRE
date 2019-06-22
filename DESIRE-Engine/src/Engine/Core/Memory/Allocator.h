#pragma once

// --------------------------------------------------------------------------------------------------------------------
//	Base class for memory allocators
// --------------------------------------------------------------------------------------------------------------------

class Allocator
{
public:
	static constexpr size_t kDefaultAlignment = 8;

	Allocator() {}
	virtual ~Allocator() {}

	virtual void* Alloc(size_t size) = 0;
	virtual void Free(void* ptr) = 0;

	// Returns the default SystemMemoryAllocator
	static Allocator& GetDefaultAllocator();

	// Returns a linear allocator which gets reset at the end of each frame
	static Allocator& GetScratchAllocator();

	// Reset all allocations in the frame allocator (this should happen at the end of the frame)
	static void ResetScratchAllocator();

private:
	// Prevent copy and move
	Allocator(const Allocator& other) = delete;
	Allocator(Allocator&& other) = delete;
	Allocator& operator=(const Allocator& other) = delete;
	Allocator& operator=(Allocator&& other) = delete;
};
