#include "Engine/stdafx.h"
#include "Engine/Utils/TaskManager.h"

TaskManager::TaskManager()
{

}

TaskManager::~TaskManager()
{

}

void TaskManager::AddTask(std::function<void()> task, bool toFront)
{
	ASSERT(task != nullptr);

	std::lock_guard<std::mutex> lock(mutex);
	if(toFront)
	{
		taskQueue.push_front(task);
	}
	else
	{
		taskQueue.push_back(task);
	}
}

void TaskManager::AddDelayedTask(float delaySecs, std::function<void()> task)
{
	ASSERT(task != nullptr);

	std::lock_guard<std::mutex> lock(mutex);
	timedTasks.push({ timer + delaySecs, task });
}

void TaskManager::Update(float deltaTime)
{
	// Check timed tasks
	if(!timedTasks.empty())
	{
		mutex.lock();

		timer += deltaTime;

		while(!timedTasks.empty() && timedTasks.top().time < timer)
		{
			std::function<void()> task = timedTasks.top().task;
			timedTasks.pop();

			mutex.unlock();
			task();
			mutex.lock();
		}

		// Reset timer if we run out of tasks
		if(timedTasks.empty())
		{
			timer = 0.0f;
		}

		mutex.unlock();
	}

	// Check normal tasks
	if(!taskQueue.empty())
	{
		mutex.lock();

		if(!taskQueue.empty())	// Just to be sure
		{
			std::function<void()> task = taskQueue.front();
			taskQueue.pop_front();

			mutex.unlock();
			task();
		}
		else
		{
			mutex.unlock();
		}
	}
}

void TaskManager::Clear()
{
	std::lock_guard<std::mutex> lock(mutex);

	taskQueue.clear();

	while(!timedTasks.empty())
	{
		timedTasks.pop();
	}

	timer = 0.0f;
}
