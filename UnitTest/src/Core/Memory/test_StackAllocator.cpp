#include "stdafx.h"
#include "Engine/Core/Memory/StackAllocator.h"
#include "Engine/Core/Memory/MemorySystem.h"

TEST_CASE("StackAllocator", "[Core][memory]")
{
	StackAllocator<128> a;

	const void* memoryStart = (char*)& a + sizeof(LinearAllocator);
	const void* memoryEnd = (char*)memoryStart + (sizeof(a) - sizeof(LinearAllocator));

	SECTION("Default alignment")
	{
		CHECK(a.GetAllocatedBytes() == 0);
		void* ptr1 = a.Alloc(1);
		CHECK(a.GetAllocatedBytes() == 1);
		void* ptr2 = a.Alloc(1);
		CHECK(a.GetAllocatedBytes() >= 2);

		CHECK(reinterpret_cast<uintptr_t>(ptr1) % MemorySystem::kDefaultAlignment == 0);
		CHECK(reinterpret_cast<uintptr_t>(ptr2) % MemorySystem::kDefaultAlignment == 0);
	}

	SECTION("Test Fallback Allocator 1/4")
	{
		// ptrFB is supposed to be allocated via the fallback allocator
		CHECK(a.GetAllocatedBytes() == 0);
		void* ptr1 = a.Alloc(64);
		void* ptr2 = a.Alloc(64);
		CHECK(a.GetAllocatedBytes() == 2 * 64);
		void* ptrFB = a.Alloc(10);
		CHECK(a.GetAllocatedBytes() == 2 * 64);

		CHECK((memoryStart <= ptr1 && ptr1 < memoryEnd));
		CHECK((memoryStart <= ptr2 && ptr2 < memoryEnd));
		CHECK_FALSE((memoryStart <= ptrFB && ptrFB < memoryEnd));

		a.Free(ptr1, 64);
		a.Free(ptr2, 64);
		a.Free(ptrFB, 10);
	}

	SECTION("Test Fallback Allocator 2/4")
	{
		CHECK(a.GetAllocatedBytes() == 0);
		void* ptr1 = a.Alloc(64);
		CHECK(a.GetAllocatedBytes() == 64);
		void* ptrFB = a.Alloc(100);
		CHECK(a.GetAllocatedBytes() == 64);
		void* ptr2 = a.Alloc(10);
		CHECK(a.GetAllocatedBytes() == 74);

		CHECK((memoryStart <= ptr1 && ptr1 < memoryEnd));
		CHECK((memoryStart <= ptr2 && ptr2 < memoryEnd));
		CHECK_FALSE((memoryStart <= ptrFB && ptrFB < memoryEnd));

		a.Free(ptr1, 64);
		a.Free(ptr2, 10);
		a.Free(ptrFB, 100);
	}

	SECTION("Test Fallback Allocator 3/4")
	{
		CHECK(a.GetAllocatedBytes() == 0);
		void* ptrFB = a.Alloc(200);
		CHECK(a.GetAllocatedBytes() == 0);
		CHECK_FALSE((memoryStart <= ptrFB && ptrFB < memoryEnd));

		a.Free(ptrFB, 200);
	}

	SECTION("Test Fallback Allocator 4/4")
	{
		CHECK(a.GetAllocatedBytes() == 0);
		void* ptr1 = a.Alloc(113);
		CHECK(a.GetAllocatedBytes() == 113);
		CHECK((memoryStart <= ptr1 && ptr1 < memoryEnd));

		// Because of the default alignment the second allocation cannot be made inside the allocator regardless of the fact that the overall size would be only 128
		void* ptrFB = a.Alloc(15);
		CHECK(a.GetAllocatedBytes() == 113);
		CHECK_FALSE((memoryStart <= ptrFB && ptrFB < memoryEnd));

		a.Free(ptr1, 113);
		a.Free(ptrFB, 15);
	}
}
