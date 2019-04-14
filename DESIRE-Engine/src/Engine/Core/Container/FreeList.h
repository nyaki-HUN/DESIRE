#pragma once

// --------------------------------------------------------------------------------------------------------------------
//	FreeList operates by connecting unused elements together in a linked list, using the first few bytes as a pointer to the next.
//	https://en.wikipedia.org/wiki/Free_list
// --------------------------------------------------------------------------------------------------------------------

class FreeList
{
public:
	void Push(void *element);
	void* Pop();

private:
	struct ListElement
	{
		ListElement *next;
	};

	ListElement *head = nullptr;
};
