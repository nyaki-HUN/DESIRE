#pragma once

class SpinLock
{
public:
	void Lock();
	bool TryLock();
	void Unlock();
	bool IsLocked() const;

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
