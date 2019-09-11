#include "Engine/stdafx.h"
#include "Engine/Utils/TaskManager.h"

TaskManager::TaskManager()
{
}

TaskManager::~TaskManager()
{
}

uint32_t TaskManager::AddTask(std::function<void()> task, bool toFront)
{
	ASSERT(task != nullptr);

	std::lock_guard<std::mutex> lock(taskQueueMutex);
	const uint32_t taskId = taskUniqueId++;
	if(toFront)
	{
		taskQueue.emplace_front(taskId, task);
	}
	else
	{
		taskQueue.emplace_back(taskId, task);
	}

	return taskId;
}

void TaskManager::CancelTask(uint32_t taskId)
{
	std::lock_guard<std::mutex> lock(taskQueueMutex);
	for(auto it = taskQueue.begin(); it != taskQueue.end(); ++it)
	{
		if(taskId == it->first)
		{
			taskQueue.erase(it);
			return;
		}
	}
}

void TaskManager::AddDelayedTask(float delaySecs, std::function<void()> task)
{
	ASSERT(task != nullptr);

	std::lock_guard<std::mutex> lock(taskQueueMutex);
	timedTasks.push({ timer + delaySecs, task });
}

void TaskManager::Update(float deltaTime)
{
	// Check timed tasks
	if(!timedTasks.empty())
	{
		taskQueueMutex.lock();

		timer += deltaTime;

		while(!timedTasks.empty() && timedTasks.top().time < timer)
		{
			std::function<void()> task = timedTasks.top().task;
			timedTasks.pop();

			taskQueueMutex.unlock();
			task();
			taskQueueMutex.lock();
		}

		// Reset timer if we run out of tasks
		if(timedTasks.empty())
		{
			timer = 0.0f;
		}

		taskQueueMutex.unlock();
	}

	// Check normal tasks
	if(!taskQueue.empty())
	{
		taskQueueMutex.lock();

		if(!taskQueue.empty())	// Just to be sure
		{
			std::function<void()> task = taskQueue.front().second;
			taskQueue.pop_front();

			taskQueueMutex.unlock();
			task();
		}
		else
		{
			taskQueueMutex.unlock();
		}
	}
}

void TaskManager::Clear()
{
	std::lock_guard<std::mutex> lock(taskQueueMutex);

	taskQueue.clear();

	while(!timedTasks.empty())
	{
		timedTasks.pop();
	}

	timer = 0.0f;
	taskUniqueId = 0;
}
