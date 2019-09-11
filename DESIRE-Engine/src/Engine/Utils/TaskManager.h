#pragma once

#include <mutex>
#include <deque>
#include <queue>		// for std::priority_queue
#include <functional>

class TaskManager
{
public:
	TaskManager();
	~TaskManager();

	uint32_t AddTask(std::function<void()> task, bool toFront = false);
	void CancelTask(uint32_t taskId);

	void AddDelayedTask(float delaySecs, std::function<void()> task);

	void Update(float deltaTime);
	void Clear();

private:
	struct TimedTask
	{
		float time;
		std::function<void(void)> task;

		bool operator >(const TimedTask& other) const
		{
			return time > other.time;
		}
	};

	std::mutex taskQueueMutex;
	std::deque<std::pair<uint32_t, std::function<void()>>> taskQueue;
	std::priority_queue<TimedTask, std::vector<TimedTask>, std::greater<TimedTask>> timedTasks;
	float timer = 0.0f;
	uint32_t taskUniqueId = 0;
};
