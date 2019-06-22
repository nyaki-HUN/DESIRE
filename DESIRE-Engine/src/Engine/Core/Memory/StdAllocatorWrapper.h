#pragma once

#include "Engine/Core/platform.h"			// for DESIRE_UNUSED
#include "Engine/Core/Memory/Allocator.h"

template<typename T>
class StdAllocatorWrapper
{
public:
	typedef T value_type;

	StdAllocatorWrapper()
	{
	}

	template<typename U>
	StdAllocatorWrapper(const StdAllocatorWrapper<U>&)
	{
	}

	T* allocate(size_t n) const
	{
		if(n == 0)
		{
			return nullptr;
		}

		void* ptr = Allocator::GetScratchAllocator().Alloc(n * sizeof(T));
		return static_cast<T*>(ptr);
	}

	void deallocate(T* ptr, size_t n) const
	{
		DESIRE_UNUSED(n);
		Allocator::GetScratchAllocator().Free(ptr);
	}

	// Stateless allocators are always equal
	bool operator ==(const StdAllocatorWrapper& other) const	{ return true; }
	bool operator !=(const StdAllocatorWrapper& other) const	{ return false; }

private:
	// Prevent copy and move
	StdAllocatorWrapper(const StdAllocatorWrapper& other) = delete;
	StdAllocatorWrapper(StdAllocatorWrapper&& other) = delete;
	StdAllocatorWrapper& operator=(const StdAllocatorWrapper& other) = delete;
	StdAllocatorWrapper& operator=(StdAllocatorWrapper&& other) = delete;
};
