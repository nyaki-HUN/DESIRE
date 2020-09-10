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
		DESIRE_NO_COPY_AND_MOVE(ScopeLock)

		SpinLock& spinLock;
	};

private:
	std::atomic<bool> locked = false;

	static constexpr int kMaxIterations = 16;
};

#define DESIRE_SCOPED_SPINLOCK(LOCK)	SpinLock::ScopeLock DESIRE_CONCAT_MACRO(scopeLock, __LINE__) (LOCK)
