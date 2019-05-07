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

		void *ptr = Allocator::GetFrameAllocator().Allocate(n * sizeof(T));
		return static_cast<T*>(ptr);
	}

	void deallocate(T *ptr, size_t n) const
	{
		DESIRE_UNUSED(n);
		Allocator::GetFrameAllocator().Deallocate(ptr);
	}

	// Stateless allocators are always equal
	bool operator ==(const StdAllocatorWrapper& other) const
	{
		return true;
	}

	// Stateless allocators are always equal
	bool operator !=(const StdAllocatorWrapper& other) const
	{
		return false;
	}

private:
	StdAllocatorWrapper& operator =(const StdAllocatorWrapper&) = delete;
};
