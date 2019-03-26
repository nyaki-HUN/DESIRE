#pragma once

// --------------------------------------------------------------------------------------------------------------------
//	FreeList is a data structure used in PoolAllocator for memory handling.
//	It operates by connecting unallocated regions of memory together in a linked list, using the first few bytes of each unallocated region as a pointer to the next.
//	https://en.wikipedia.org/wiki/Free_list
// --------------------------------------------------------------------------------------------------------------------

class FreeList
{
public:
	FreeList(void *memoryStart, size_t memorySize, size_t elementSize);

	void* ObtainElement();
	void ReturnElement(void *element);

private:
	struct ListElement
	{
		ListElement *next;
	};

	ListElement *head = nullptr;
};
