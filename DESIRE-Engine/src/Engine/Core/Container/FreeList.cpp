#include "Engine/stdafx.h"
#include "Engine/Core/Container/FreeList.h"

void FreeList::Push(void *element)
{
	ASSERT(element != nullptr);

	// Put the element at the head of the list
	ListElement *etmp = static_cast<ListElement*>(element);
	etmp->next = head;
	head = etmp;
}

void* FreeList::Pop()
{
	ListElement *element = head;
	if(head != nullptr)
	{
		head = head->next;
	}

	return element;
}
