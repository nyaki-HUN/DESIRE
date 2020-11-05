#pragma once

#include "Engine/Core/Container/FreeList.h"
#include "Engine/Core/SpinLock.h"

class ThreadSafeFreeList
{
public:
	void Push(void* element)
	{
		DESIRE_SCOPED_SPINLOCK(m_spinLock);
		m_freeList.Push(element);
	}

	void* Pop()
	{
		DESIRE_SCOPED_SPINLOCK(m_spinLock);
		return m_freeList.Pop();
	}

private:
	SpinLock m_spinLock;
	FreeList m_freeList;
};
