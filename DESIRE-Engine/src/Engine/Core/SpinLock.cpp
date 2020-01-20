#include "Engine/stdafx.h"
#include "Engine/Core/SpinLock.h"

#include <emmintrin.h>	// for _mm_pause()
#include <thread>

void SpinLock::Lock()
{
	// Loop until we acquire the lock
	for(int spinCount = 0; !TryLock(); ++spinCount)
	{
		if(spinCount < kMaxIterations)
		{
			_mm_pause();
		}
		else
		{
			std::this_thread::yield();
			spinCount = 0;
		}
	}
}

bool SpinLock::TryLock()
{
	return !locked.load(std::memory_order_relaxed) && !locked.exchange(true, std::memory_order_acquire);
}

void SpinLock::Unlock()
{
	locked.store(false, std::memory_order_release);
}

bool SpinLock::IsLocked() const
{
	return locked.load(std::memory_order_relaxed);
}
