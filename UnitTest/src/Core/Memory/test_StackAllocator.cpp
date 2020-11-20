#include "stdafx.h"
#include "Engine/Core/Memory/StackAllocator.h"
#include "Engine/Core/Memory/MemorySystem.h"

TEST_CASE("StackAllocator", "[Core][memory]")
{
	StackAllocator<128> a;

	SECTION("Default alignment")
	{
		CHECK(a.GetAllocatedBytes() == 0);
		void* pAlloc1 = a.Alloc(1);
		CHECK(a.GetAllocatedBytes() == 1);
		void* pAlloc2 = a.Alloc(1);
		CHECK(a.GetAllocatedBytes() == MemorySystem::kDefaultAlignment + 1);

		CHECK(a.IsMemoryFromThis(pAlloc1));
		CHECK(a.IsMemoryFromThis(pAlloc2));

		CHECK(reinterpret_cast<uintptr_t>(pAlloc1) % MemorySystem::kDefaultAlignment == 0);
		CHECK(reinterpret_cast<uintptr_t>(pAlloc2) % MemorySystem::kDefaultAlignment == 0);
	}

	SECTION("Test Fallback Allocator 1/4")
	{
		// The pFallbackAlloc is supposed to be allocated via the fallback allocator
		CHECK(a.GetAllocatedBytes() == 0);
		void* pAlloc1 = a.Alloc(64);
		void* pAlloc2 = a.Alloc(64);
		CHECK(a.GetAllocatedBytes() == 2 * 64);
		void* pFallbackAlloc = a.Alloc(10);
		CHECK(a.GetAllocatedBytes() == 2 * 64);

		CHECK(a.IsMemoryFromThis(pAlloc1));
		CHECK(a.IsMemoryFromThis(pAlloc2));
		CHECK_FALSE(a.IsMemoryFromThis(pFallbackAlloc));

		a.Free(pAlloc1, 64);
		a.Free(pAlloc2, 64);
		a.Free(pFallbackAlloc, 10);
	}

	SECTION("Test Fallback Allocator 2/4")
	{
		CHECK(a.GetAllocatedBytes() == 0);
		void* pAlloc1 = a.Alloc(64);
		CHECK(a.GetAllocatedBytes() == 64);
		void* pFallbackAlloc = a.Alloc(100);
		CHECK(a.GetAllocatedBytes() == 64);
		void* pAlloc2 = a.Alloc(10);
		CHECK(a.GetAllocatedBytes() == 74);

		CHECK(a.IsMemoryFromThis(pAlloc1));
		CHECK(a.IsMemoryFromThis(pAlloc2));
		CHECK_FALSE(a.IsMemoryFromThis(pFallbackAlloc));

		a.Free(pAlloc1, 64);
		a.Free(pAlloc2, 10);
		a.Free(pFallbackAlloc, 100);
	}

	SECTION("Test Fallback Allocator 3/4")
	{
		CHECK(a.GetAllocatedBytes() == 0);
		void* pFallbackAlloc = a.Alloc(200);
		CHECK(a.GetAllocatedBytes() == 0);
		CHECK_FALSE(a.IsMemoryFromThis(pFallbackAlloc));

		a.Free(pFallbackAlloc, 200);
	}

	SECTION("Test Fallback Allocator 4/4")
	{
		CHECK(a.GetAllocatedBytes() == 0);
		void* pAlloc1 = a.Alloc(113);
		CHECK(a.GetAllocatedBytes() == 113);
		CHECK(a.IsMemoryFromThis(pAlloc1));

		// Because of the default alignment the second allocation cannot be made inside the allocator regardless of the fact that the overall size would be only 128
		void* pFallbackAlloc = a.Alloc(15);
		CHECK(a.GetAllocatedBytes() == 113);
		CHECK_FALSE(a.IsMemoryFromThis(pFallbackAlloc));

		a.Free(pAlloc1, 113);
		a.Free(pFallbackAlloc, 15);
	}
}
