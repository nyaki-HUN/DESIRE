#pragma once

#include "Core/Singleton.h"

#include <mutex>
#include <deque>
#include <queue>		// for std::priority_queue
#include <functional>

class TaskManager
{
	DESIRE_SINGLETON(TaskManager)

public:
	void AddTask(std::function<void()> task, bool toFront = false);
	void AddDelayedTask(float delaySecs, std::function<void()> task);

	void Update();
	void Clear();

private:
	struct TimedTask
	{
		float time;
		std::function<void(void)> task;

		bool operator >(const TimedTask &other) const
		{
			return time > other.time;
		}
	};

	std::mutex mutex;
	std::deque<std::function<void()>> taskQueue;
	std::priority_queue<TimedTask, std::vector<TimedTask>, std::greater<TimedTask>> timedTasks;
	float timer;
};
