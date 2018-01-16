#include "stdafx.h"
#include "Core/memory/FreeList.h"

FreeList::FreeList(void *memoryStart, size_t memorySize, size_t elementSize)
{
	ASSERT(elementSize >= sizeof(void*));
	char *ptr = (char*)memoryStart;

	const size_t numElements = memorySize / elementSize;
	FreeList *tmp = this;
	for(size_t i = 0; i < numElements; ++i)
	{
		tmp->next = (FreeList*)ptr;
		ptr += elementSize;
		tmp = tmp->next;
	}

	tmp->next = nullptr;
}

void* FreeList::ObtainElement()
{
	FreeList *element = next;
	if(next != nullptr)
	{
		next = next->next;
	}

	return element;
}

void FreeList::ReturnElement(void *element)
{
	ASSERT(element != nullptr);

	// Put the element at the head of the list
	FreeList *ptr = static_cast<FreeList*>(element);
	ptr->next = next;
	next = ptr;
}
