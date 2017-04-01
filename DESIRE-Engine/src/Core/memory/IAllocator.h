#pragma once

// --------------------------------------------------------------------------------------------------------------------
//	Base class for memory allocators
// --------------------------------------------------------------------------------------------------------------------

class IAllocator
{
public:
	static const size_t DEFAULT_ALIGNMENT = 8;

	IAllocator() {}
	virtual ~IAllocator() {}

	virtual void* Allocate(size_t size, size_t alignment = IAllocator::DEFAULT_ALIGNMENT) = 0;
	virtual void Deallocate(void *ptr) = 0;

	// Aligns ptr to the specified alignment by moving it forward
	static void* AlignForward(void *ptr, size_t alignment);

	// Returns the default MallocAllocator
	static IAllocator& GetDefaultAllocator();

	// Returns a linear allocator which is reset at the end of each frame
	static IAllocator& GetFrameAllocator();

	// Reset all allocations in the frame allocator (this should happen at the end of the frame)
	static void ResetFrameAllocator();

private:
	// Prevent copy
	IAllocator(const IAllocator& other) = delete;
	IAllocator& operator=(const IAllocator& other) = delete;
};

// Creates a new object of type T using the allocator 'A' to allocate its memory
#define DESIRE_ALLOCATOR_NEW(A, T, ...)			( new ((A).Allocate(sizeof(T), alignof(T))) T(__VA_ARGS__) )

// Destroys an object allocated with DESIRE_ALLOCATOR_NEW
#define DESIRE_ALLOCATOR_DELETE(A, T, PTR)		do{ if(PTR) { (PTR)->~T(); A.Deallocate(PTR); } }while(0)
