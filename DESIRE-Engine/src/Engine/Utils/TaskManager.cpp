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

	std::lock_guard<std::mutex> lock(m_taskQueueMutex);
	const uint32_t taskId = m_taskUniqueId++;
	if(toFront)
	{
		m_taskQueue.emplace_front(taskId, task);
	}
	else
	{
		m_taskQueue.emplace_back(taskId, task);
	}

	return taskId;
}

void TaskManager::CancelTask(uint32_t taskId)
{
	std::lock_guard<std::mutex> lock(m_taskQueueMutex);
	for(auto it = m_taskQueue.begin(); it != m_taskQueue.end(); ++it)
	{
		if(taskId == it->first)
		{
			m_taskQueue.erase(it);
			return;
		}
	}
}

void TaskManager::AddDelayedTask(float delaySecs, std::function<void()> task)
{
	ASSERT(task != nullptr);

	std::lock_guard<std::mutex> lock(m_taskQueueMutex);
	m_timedTasks.push({ m_timer + delaySecs, task });
}

void TaskManager::Update(float deltaTime)
{
	// Check timed tasks
	if(!m_timedTasks.empty())
	{
		m_taskQueueMutex.lock();

		m_timer += deltaTime;

		while(!m_timedTasks.empty() && m_timedTasks.top().time < m_timer)
		{
			std::function<void()> task = m_timedTasks.top().task;
			m_timedTasks.pop();

			m_taskQueueMutex.unlock();
			task();
			m_taskQueueMutex.lock();
		}

		// Reset m_timer if we run out of tasks
		if(m_timedTasks.empty())
		{
			m_timer = 0.0f;
		}

		m_taskQueueMutex.unlock();
	}

	// Check normal tasks
	if(!m_taskQueue.empty())
	{
		m_taskQueueMutex.lock();

		if(!m_taskQueue.empty())	// Just to be sure
		{
			std::function<void()> task = m_taskQueue.front().second;
			m_taskQueue.pop_front();

			m_taskQueueMutex.unlock();
			task();
		}
		else
		{
			m_taskQueueMutex.unlock();
		}
	}
}

void TaskManager::Clear()
{
	std::lock_guard<std::mutex> lock(m_taskQueueMutex);

	m_taskQueue.clear();

	while(!m_timedTasks.empty())
	{
		m_timedTasks.pop();
	}

	m_timer = 0.0f;
	m_taskUniqueId = 0;
}
