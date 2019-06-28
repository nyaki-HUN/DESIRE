#include "Engine/stdafx.h"
#include "Engine/Core/Memory/MemorySystem.h"
#include "Engine/Core/Memory/LinearAllocator.h"

thread_local Allocator* MemorySystem::allocatorStack[kAllocatorStackSize] = {};
thread_local size_t MemorySystem::allocatorStackIndex = 0;

// Operator new/delete overrides
void* operator new  (size_t size)											{ return MemorySystem::Alloc(size); }
void* operator new[](size_t size)											{ return MemorySystem::Alloc(size); }
void* operator new  (size_t size, std::align_val_t alignment)				{ return MemorySystem::AlignedAlloc(size, static_cast<std::size_t>(alignment)); }
void* operator new[](size_t size, std::align_val_t alignment)				{ return MemorySystem::AlignedAlloc(size, static_cast<std::size_t>(alignment)); }

// Operator delete overrides
void operator delete  (void* ptr) noexcept									{ MemorySystem::Free(ptr); }
void operator delete[](void* ptr) noexcept									{ MemorySystem::Free(ptr); }
void operator delete  (void* ptr, std::align_val_t /*alignment*/) noexcept	{ MemorySystem::AlignedFree(ptr); }
void operator delete[](void* ptr, std::align_val_t /*alignment*/) noexcept	{ MemorySystem::AlignedFree(ptr); }

void* MemorySystem::Alloc(size_t size)
{
	ASSERT(size != 0);

	Allocator& allocator = GetActiveAllocator();
	return allocator.Alloc(size);
}

void* MemorySystem::Calloc(size_t num, size_t size)
{
	void* ptr = Alloc(num * size);
	if(ptr != nullptr)
	{
		memset(ptr, 0, num * size);
	}
	return ptr;
}

void* MemorySystem::Realloc(void* ptr, size_t size)
{
	ASSERT(size != 0);

	Allocator& allocator = GetActiveAllocator();
	return allocator.Realloc(ptr, size);
}

void* MemorySystem::AlignedAlloc(size_t size, size_t alignment)
{
	return SystemAlignedAlloc(size, alignment);
}

void MemorySystem::Free(void* ptr)
{
	if(ptr == nullptr)
	{
		return;
	}

	Allocator& allocator = GetActiveAllocator();
	return allocator.Free(ptr);
}

void MemorySystem::AlignedFree(void* ptr)
{
	if(ptr == nullptr)
	{
		return;
	}

	return SystemAlignedFree(ptr);
}

Allocator& MemorySystem::GetActiveAllocator()
{
	return (allocatorStackIndex > 0) ? *allocatorStack[allocatorStackIndex - 1] : Allocator::GetDefaultAllocator();
}

void MemorySystem::PushAllocator(Allocator& allocator)
{
	ASSERT(allocatorStackIndex < kAllocatorStackSize);
	allocatorStack[allocatorStackIndex++] = &allocator;
}

void MemorySystem::PopAllocator()
{
	if(allocatorStackIndex > 0)
	{
		allocatorStackIndex--;
	}
	else
	{
		ASSERT(false);
	}
}

Allocator& MemorySystem::GetScratchAllocator()
{
	static constexpr size_t kScratchAllocatorSize = 10 * 1024 * 1024;
	static std::unique_ptr<char[]> data = std::make_unique<char[]>(kScratchAllocatorSize);
	static LinearAllocator s_scratchAllocator(data.get(), kScratchAllocatorSize);
	return s_scratchAllocator;
}

void MemorySystem::ResetScratchAllocator()
{
	static_cast<LinearAllocator&>(GetScratchAllocator()).Reset();
}
