#pragma once

#include "Engine/Core/platform.h"			// for DESIRE_UNUSED
#include "Engine/Core/Memory/Allocator.h"

#include <new>
#include <utility>			// for std::forward()

template<typename T>
class StdAllocatorWrapper
{
public:
	typedef T value_type;
	// Note: The following types are deprecated in C++17
	typedef T* pointer;
	typedef const T* const_pointer;
	typedef T& reference;
	typedef const T& const_reference;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;

	// (deprecated in C++17)
	template<typename U>
	struct rebind
	{
		typedef StdAllocatorWrapper<U> other;
	};

	StdAllocatorWrapper()
	{

	}

	template<typename U>
	StdAllocatorWrapper(const StdAllocatorWrapper<U>&)
	{

	}

	// (deprecated in C++17)
	T* address(T& r) const
	{
		return std::addressof(r);
	}

	// (deprecated in C++17)
	const T* address(const T& s) const
	{
		return std::addressof(s);
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

	// (deprecated in C++17)
	T* allocate(size_t n, const void *hint) const
	{
		DESIRE_UNUSED(hint);
		return allocate(n);
	}

	void deallocate(T *ptr, size_t n) const
	{
		DESIRE_UNUSED(n);
		Allocator::GetFrameAllocator().Deallocate(ptr);
	}

	// (deprecated in C++17)
	size_t max_size() const
	{
		return SIZE_MAX / sizeof(T);
	}

	// (deprecated in C++17)
	template<class U, class... Args>
	void construct(U *ptr, Args&&... args)
	{
		new (ptr) U(std::forward<Args>(args)...);
	}

	// (deprecated in C++17)
	template<class U>
	void destroy(U *ptr)
	{
		DESIRE_UNUSED(ptr);		// Suppress warning C4100: 'ptr': unreferenced formal parameter
		ptr->~U();
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
