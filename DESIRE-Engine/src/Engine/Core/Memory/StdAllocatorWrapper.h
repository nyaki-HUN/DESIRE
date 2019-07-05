#pragma once

#include "Engine/Core/Memory/MemorySystem.h"

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

		void* ptr = MemorySystem::Alloc(n * sizeof(T));
		return static_cast<T*>(ptr);
	}

	void deallocate(T* ptr, size_t /*n*/) const
	{
		MemorySystem::Free(ptr);
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
