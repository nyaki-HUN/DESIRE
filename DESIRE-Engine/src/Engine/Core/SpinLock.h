#pragma once

#include <atomic>
#include <emmintrin.h>	// for _mm_pause()
#include <thread>

class SpinLock
{
public:
	void Lock()
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
	bool TryLock()
	{
		return !locked.load(std::memory_order_relaxed) && !locked.exchange(true, std::memory_order_acquire);
	}

	void Unlock()
	{
		locked.store(false, std::memory_order_release);
	}

	bool IsLocked() const
	{
		return locked.load(std::memory_order_relaxed);
	}

	class ScopeLock
	{
	public:
		ScopeLock(SpinLock& spinLock)
			: spinLock(spinLock)
		{
			spinLock.Lock();
		}

		~ScopeLock()
		{
			spinLock.Unlock();
		}

	private:
		// Prevent copy and move
		ScopeLock(const ScopeLock&) = delete;
		ScopeLock(ScopeLock&&) = delete;
		ScopeLock& operator =(const ScopeLock&) = delete;
		ScopeLock& operator =(ScopeLock&&) = delete;

		SpinLock& spinLock;
	};

private:
	std::atomic<bool> locked = false;

	static constexpr int kMaxIterations = 16;
};

#define DESIRE_SCOPED_SPINLOCK(LOCK)	SpinLock::ScopeLock DESIRE_CONCAT_MACRO(scopeLock, __LINE__) (LOCK)
