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

	virtual void* Allocate(size_t size, size_t alignment = Allocator::kDefaultAlignment) = 0;
	virtual void Deallocate(void *ptr) = 0;

	// Returns the default MallocAllocator
	static Allocator& GetDefaultAllocator();

	// Returns a linear allocator which is reset at the end of each frame
	static Allocator& GetFrameAllocator();

	// Reset all allocations in the frame allocator (this should happen at the end of the frame)
	static void ResetFrameAllocator();

private:
	// Prevent copy
	Allocator(const Allocator& other) = delete;
	Allocator& operator=(const Allocator& other) = delete;
};

// Creates a new object of type T using the allocator 'A' to allocate its memory
#define DESIRE_ALLOCATOR_NEW(A, T, ...)			( new ((A).Allocate(sizeof(T), alignof(T))) T(__VA_ARGS__) )

// Destroys an object allocated with DESIRE_ALLOCATOR_NEW
#define DESIRE_ALLOCATOR_DELETE(A, T, PTR)		do{ if(PTR) { (PTR)->~T(); A.Deallocate(PTR); } }while(0)
