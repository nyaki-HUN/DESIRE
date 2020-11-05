#include "Engine/stdafx.h"
#include "Engine/Core/Container/FreeList.h"

void FreeList::Push(void* pElement)
{
	ASSERT(pElement != nullptr);

	// Put the element at the head of the list
	ListElement* pEtmp = static_cast<ListElement*>(pElement);
	pEtmp->pNext = m_pHead;
	m_pHead = pEtmp;
}

void* FreeList::Pop()
{
	ListElement* pElement = m_pHead;
	if(m_pHead)
	{
		m_pHead = m_pHead->pNext;
	}

	return pElement;
}
