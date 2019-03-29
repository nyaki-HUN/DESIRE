#include "Engine/stdafx.h"
#include "Engine/Core/Memory/FreeList.h"

FreeList::FreeList(void *memoryStart, size_t memorySize, size_t elementSize)
{
	ASSERT(elementSize >= sizeof(void*));
	ASSERT(memorySize >= elementSize);

	char *ptr = static_cast<char*>(memoryStart);
	const size_t numElements = memorySize / elementSize;

	head = reinterpret_cast<ListElement*>(ptr);
	ptr += elementSize;

	ListElement *etmp = head;
	for(size_t i = 1; i < numElements; ++i)
	{
		etmp->next = reinterpret_cast<ListElement*>(ptr);
		ptr += elementSize;
		etmp = etmp->next;
	}

	etmp->next = nullptr;
}

void* FreeList::ObtainElement()
{
	ListElement *element = head;
	if(head != nullptr)
	{
		head = head->next;
	}

	return element;
}

void FreeList::ReturnElement(void *element)
{
	ASSERT(element != nullptr);

	// Put the element at the head of the list
	ListElement *etmp = static_cast<ListElement*>(element);
	etmp->next = head;
	head = etmp;
}
