#include "Engine/stdafx.h"
#include "Engine/Core/Memory/MemorySystem.h"

// Global operator new/delete overrides
// Note: The behavior is undefined if more than one replacement is provided in the program or if a replacement is defined with the inline specifier.
void* operator new(size_t size)
{
	void* pMemory = MemorySystem::Alloc(size, __STDCPP_DEFAULT_NEW_ALIGNMENT__);
	if(pMemory)
	{
		return pMemory;
	}

	throw std::bad_alloc{};
}

void* operator new[](size_t size)
{
	void* pMemory = MemorySystem::Alloc(size, __STDCPP_DEFAULT_NEW_ALIGNMENT__);
	if(pMemory)
	{
		return pMemory;
	}

	throw std::bad_alloc{};
}

void* operator new(size_t size, std::align_val_t alignment)
{
	void* pMemory = MemorySystem::Alloc(size, static_cast<std::size_t>(alignment));
	if(pMemory)
	{
		return pMemory;
	}

	throw std::bad_alloc{};
}

void* operator new[](size_t size, std::align_val_t alignment)
{
	void* pMemory = MemorySystem::Alloc(size, static_cast<std::size_t>(alignment));
	if(pMemory)
	{
		return pMemory;
	}

	throw std::bad_alloc{};
}

// Non-throwing allocation functions
void* operator new  (size_t size, const std::nothrow_t&) noexcept										{ return MemorySystem::Alloc(size, __STDCPP_DEFAULT_NEW_ALIGNMENT__); }
void* operator new[](size_t size, const std::nothrow_t&) noexcept										{ return MemorySystem::Alloc(size, __STDCPP_DEFAULT_NEW_ALIGNMENT__); }
void* operator new  (size_t size, std::align_val_t alignment, const std::nothrow_t&) noexcept			{ return MemorySystem::Alloc(size, static_cast<std::size_t>(alignment)); }
void* operator new[](size_t size, std::align_val_t alignment, const std::nothrow_t&) noexcept			{ return MemorySystem::Alloc(size, static_cast<std::size_t>(alignment)); }

// Usual deallocation functions
void operator delete  (void* pMemory) noexcept															{ MemorySystem::Free(pMemory); }
void operator delete[](void* pMemory) noexcept															{ MemorySystem::Free(pMemory); }
void operator delete  (void* pMemory, std::align_val_t /*alignment*/) noexcept							{ MemorySystem::Free(pMemory); }
void operator delete[](void* pMemory, std::align_val_t /*alignment*/) noexcept							{ MemorySystem::Free(pMemory); }
// Non-throwing deallocation functions
void operator delete  (void* pMemory, const std::nothrow_t&) noexcept									{ MemorySystem::Free(pMemory); }
void operator delete[](void* pMemory, const std::nothrow_t&) noexcept									{ MemorySystem::Free(pMemory); }
void operator delete  (void* pMemory, std::align_val_t /*alignment*/, const std::nothrow_t&) noexcept	{ MemorySystem::Free(pMemory); }
void operator delete[](void* pMemory, std::align_val_t /*alignment*/, const std::nothrow_t&) noexcept	{ MemorySystem::Free(pMemory); }
