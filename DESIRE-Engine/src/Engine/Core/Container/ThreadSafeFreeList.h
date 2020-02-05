#pragma once

#include "Engine/Core/Container/FreeList.h"
#include "Engine/Core/SpinLock.h"

class ThreadSafeFreeList
{
public:
	void Push(void* element)
	{
		DESIRE_SCOPED_SPINLOCK(spinLock);
		freeList.Push(element);
	}

	void* Pop()
	{
		DESIRE_SCOPED_SPINLOCK(spinLock);
		return freeList.Pop();
	}

private:
	SpinLock spinLock;
	FreeList freeList;
};
