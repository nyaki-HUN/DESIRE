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
			: m_spinLock(spinLock)
		{
			spinLock.Lock();
		}

		~ScopeLock()
		{
			m_spinLock.Unlock();
		}

	private:
		DESIRE_NO_COPY_AND_MOVE(ScopeLock)

		SpinLock& m_spinLock;
	};

private:
	std::atomic<bool> m_locked = false;

	static constexpr int kMaxIterations = 16;
};

#define DESIRE_SCOPED_SPINLOCK(LOCK)	SpinLock::ScopeLock DESIRE_CONCAT_MACRO(scopeLock, __LINE__) (LOCK)
