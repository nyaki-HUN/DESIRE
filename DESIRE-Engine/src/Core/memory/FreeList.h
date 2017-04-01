#pragma once

// --------------------------------------------------------------------------------------------------------------------
//	FreeList is a data structure used in PoolAllocator for dynamic memory allocation.
//	It operates by connecting free slots of memory together in a linked list inside the pre-allocated memory pool.
//	https://en.wikipedia.org/wiki/Free_list
// --------------------------------------------------------------------------------------------------------------------

class FreeList
{
public:
	FreeList(void *memoryStart, size_t memorySize, size_t elementSize);

	void* ObtainElement();
	void ReturnElement(void *element);

private:
	FreeList *next;
};
